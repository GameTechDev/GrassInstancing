/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTsettingsdlg.h"
#include "SDKmisc.h"

#include "DX10Base.h"
#include "DX10BaseHelper.h"
#include "GrassUIState.h"
#ifdef ALT_CAMERA
	#include "camera.h"
#else
	#include "DXUTcamera.h"
#endif // ALT_CAMERA


extern CD3DSettingsDlg	g_D3DSettingsDlg; 
bool SphereConeTest ( D3DXVECTOR3 sphereCenter, float radius, float fov, D3DXVECTOR3 eyePt, D3DXVECTOR3 lookAt);


int DX10Base::Run()
{
	DXUTSetCallbackD3D10DeviceAcceptable(   &DX10BaseHelper::IsDeviceAcceptable,    this );
	DXUTSetCallbackD3D10DeviceCreated(      &DX10BaseHelper::OnCreateDevice,        this );
	DXUTSetCallbackD3D10SwapChainResized(   &DX10BaseHelper::OnResizedSwapChain,    this );
	DXUTSetCallbackD3D10SwapChainReleasing( &DX10BaseHelper::OnReleasingSwapChain,  this );
	DXUTSetCallbackD3D10DeviceDestroyed(    &DX10BaseHelper::OnDestroyDevice,       this );
	DXUTSetCallbackD3D10FrameRender(        &DX10BaseHelper::OnFrameRender,         this );
	DXUTSetCallbackMsgProc(                 &DX10BaseHelper::MsgProc,               this );
	DXUTSetCallbackKeyboard(                &DX10BaseHelper::KeyboardProc,          this );
	DXUTSetCallbackFrameMove(               &DX10BaseHelper::OnFrameMove,           this );
	DXUTSetCallbackDeviceChanging(          &DX10BaseHelper::ModifyDeviceSettings,  this );
	DXUTSetCallbackDeviceRemoved(           &DX10BaseHelper::OnDeviceRemoved,       this );
	InitApp();
	DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
	DXUTCreateWindow( L"Grass" );
//	TODO::Anu replace height/width with XML parsed data.
	DXUTCreateDevice( true, 800, 600 );
	DXUTMainLoop(); // Enter into the DXUT render loop

	return DXUTGetExitCode();

}

void DX10Base::InitApp()
{
	m_UI.Init();
	pSkybox = 0;
	m_MSAASampleCount = 1;
#ifndef ALT_CAMERA
	m_pCamera = new CModelViewerCamera();
#endif
//	m_pCamera->SetRotateButtons( true, false, false );

}
//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK DX10Base::IsDeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BackBufferFormat, bool bWindowed )
{
	// No CAPS for DX10 devices :)
	return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK DX10Base::ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings   )
{
	// Turn vsync and debug device off
	pDeviceSettings->d3d10.SyncInterval = 0;
#ifndef DEBUG
	pDeviceSettings->d3d10.CreateFlags &= ~D3D10_CREATE_DEVICE_DEBUG;
#endif

	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if( s_bFirstTime )
	{
		s_bFirstTime = false;
	if( ( pDeviceSettings->ver == DXUT_D3D10_DEVICE) && (pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE ))
			DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
	}

	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK DX10Base::OnCreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc )
{
	
	HRESULT hr;
	m_UI.Create(pd3dDevice);   
	m_ptxDiffuse = NULL;
	//m_bShowHelp = TRUE;
	// Read the D3DX effect file doing basic T&L 
	WCHAR str[MAX_PATH];
	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"fx//BasicTNL.fx" ) );

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
//	dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	V_RETURN( D3DX10CreateEffectFromFile( str, NULL, NULL, "fx_4_0", dwShaderFlags, 0, 
		pd3dDevice, NULL, NULL, &m_pEffect, NULL, NULL ) );
	
	m_pfxWorld	 = m_pEffect->GetVariableByName( "World" )->AsMatrix();
	m_pfxView	 = m_pEffect->GetVariableByName( "View" )->AsMatrix();
	m_pfxProj	 = m_pEffect->GetVariableByName( "Projection" )->AsMatrix();
	m_fTimeVar	 = m_pEffect->GetVariableByName( "g_fTime" )->AsScalar();

	m_ptxDiffuse = m_pEffect->GetVariableByName( "txDiffuse" )->AsShaderResource();

#ifdef ALT_CAMERA
	D3DXVECTOR3 pos(5.0f, 5.5f, -27.5f);

	m_pCamera = new camera(	pos,				// Position Vector		
							-0.14f,-6.12f,0,	// Rotation x,y,z
							45,1.33f,			// FOV, AspectRatio
							0.3f,500.0f			// Z - near, far
							);
#else
	D3DXVECTOR3 vecEye(0.0f, 10.0f, -40.0f);
	D3DXVECTOR3 vecAt (2.0f, 4.0f, -5.0f);
	m_pCamera->SetViewParams( &vecEye, &vecAt );
	// Setup the camera's view parameters
	//D3DXQUATERNION quatRotation;
	//D3DXQuaternionRotationYawPitchRoll( &quatRotation, -0.5f, 0.7f, 0.0f );
	//m_pCamera->SetWorldQuat( quatRotation );

#endif // ALT_CAMERA


	m_bCleanUp = TRUE;
	D3DXMatrixIdentity(&m_World);
	GetMaxMSAASampleCount( pd3dDevice, pBackBufferSurfaceDesc->Format );

	return S_OK;
}

void CALLBACK DX10Base::OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl *pControl, void* pUserContext  )
{
	m_UI.OnGUIEvent(nEvent, nControlID, pControl, pUserContext);
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK DX10Base::OnFrameMove( double fTime, float fElapsedTime   )
{

	m_fTime = (float)fTime;
	m_fTimeVar->SetFloat(m_fTime);
#ifndef ALT_CAMERA
	// Update the camera's position based on user input 
	m_pCamera->FrameMove( fElapsedTime );
#else
	//if( m_UI.GUICalled )
	//	m_Mouse.Ldragging = false;

	// Update the camera movement
	float fspeed = 0.00004f;

	if (m_Mouse.Ldragging)
	{
		m_pCamera->rotx -= fspeed*(m_Mouse.y - m_Mouse.start_y);
		m_pCamera->roty -= fspeed*(m_Mouse.x - m_Mouse.start_x);
	}
	if (m_Mouse.Rdragging)
	{
		m_pCamera->position += 5 * fspeed * (m_Mouse.x - m_Mouse.start_x) * m_pCamera->right;
		m_pCamera->position -= 5 * fspeed * (m_Mouse.y - m_Mouse.start_y) * m_pCamera->up;
	}
	if (m_Mouse.Mdragging)
	{
		m_pCamera->position -= 10 * fspeed * (m_Mouse.y - m_Mouse.start_y) * m_pCamera->forward;
	} 

	fspeed = 0.125f;
	if (KEY_DOWN(VK_SHIFT))
		fspeed = 0.25f;
	if (KEY_DOWN(VK_LCONTROL))
		fspeed = 0.005f;

	if (KEY_DOWN(0x57)) // W
		m_pCamera->position += m_pCamera->forward*fspeed;
	if (KEY_DOWN(0x53)) // S
		m_pCamera->position -= m_pCamera->forward*fspeed;
	if (KEY_DOWN(0x41)) // A
		m_pCamera->position -= m_pCamera->right*fspeed;
	if (KEY_DOWN(0x44)) // D
		m_pCamera->position += m_pCamera->right*fspeed;

		m_pCamera->update();
#endif // ALT_CAMERA




}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CALLBACK DX10Base::OnFrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime   )
{
	
	//    // Clear render target and the depth stencil 
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    pd3dDevice->ClearRenderTargetView( DXUTGetD3D10RenderTargetView(), ClearColor );
    pd3dDevice->ClearDepthStencilView( DXUTGetD3D10DepthStencilView(), D3D10_CLEAR_DEPTH, 1.0, 0 );

#ifdef ALT_CAMERA

	m_Projection =  m_pCamera->proj;
	m_View		 =  m_pCamera->view;

#else
	m_World		 = *m_pCamera->GetWorldMatrix();
	m_Projection = *m_pCamera->GetProjMatrix();
	m_View		 = *m_pCamera->GetViewMatrix();

#endif	
	pSkybox->Render( &m_View, &m_Projection );

	m_pfxWorld->SetMatrix((float*)  &m_World);
	m_pfxView->SetMatrix ((float*)  &m_View);
	m_pfxProj->SetMatrix ((float*)  &m_Projection);

}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK DX10Base::OnSwapChainResized( ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc  )
{
	m_UI.OnD3D10SwapChainResized(pd3dDevice,pBackBufferSurfaceDesc);
	// Setup the camera's projection parameters

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;

	SAFE_DELETE(pSkybox);

#ifdef ALT_CAMERA
	m_pCamera->aspect = fAspectRatio;
	m_pCamera->update_lookat();
	pSkybox = new Skybox( pd3dDevice, m_pCamera->zfar);
#else
	m_pCamera->SetProjParams( D3DX_PI/4, fAspectRatio, 1.0f,300.0f );
	m_pCamera->SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	m_pCamera->SetButtonMasks( 0, MOUSE_WHEEL, MOUSE_LEFT_BUTTON | MOUSE_MIDDLE_BUTTON | MOUSE_RIGHT_BUTTON );
	pSkybox = new Skybox( pd3dDevice, m_pCamera->GetFarClip());
#endif	

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK DX10Base::OnSwapChainReleasing(  )
{
	m_UI.OnD3D10SwapChainReleasing();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK DX10Base::OnDestroyDevice(   )
{
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	//SAFE_RELEASE( g_pRasterState );
	if(m_bCleanUp)
	{
		SAFE_RELEASE( m_pEffect );
		SAFE_DELETE ( pSkybox );
		m_UI.Destroy();
		m_bCleanUp = FALSE;
	}
}

DX10Base::~DX10Base()
{
	if(m_bCleanUp)
	{
		SAFE_RELEASE(m_pEffect);
		SAFE_DELETE(pSkybox);
		m_UI.Destroy();
			m_bCleanUp = FALSE;
	}
	SAFE_DELETE ( m_pCamera );

}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK DX10Base::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing )
{
	m_UI.MsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing, NULL );
	if(*pbNoFurtherProcessing)
		return 0;

#ifdef ALT_CAMERA
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		m_Mouse.x = (int)LOWORD(lParam);
		m_Mouse.y = (int)HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
		m_Mouse.Ldragging = TRUE;
		m_Mouse.start_x = (int)LOWORD(lParam);
		m_Mouse.start_y = (int)HIWORD(lParam);
		break;
	case WM_RBUTTONDOWN:
		m_Mouse.Rdragging = TRUE;
		m_Mouse.start_x = (int)LOWORD(lParam);
		m_Mouse.start_y = (int)HIWORD(lParam);
		break;
	case WM_MBUTTONDOWN:
		m_Mouse.Mdragging = TRUE;
		m_Mouse.start_x = (int)LOWORD(lParam);
		m_Mouse.start_y = (int)HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
		m_Mouse.Ldragging = FALSE;
		break;
	case WM_RBUTTONUP:
		m_Mouse.Rdragging = FALSE;
		break;
	case WM_MBUTTONUP:
		m_Mouse.Mdragging = FALSE;
		break;
	}
#else
	m_pCamera->HandleMessages( hWnd, uMsg, wParam, lParam );
#endif	

	return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK DX10Base::KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown )
{
		if( bKeyDown )
		{
			switch( nChar )
			{
			case VK_F1:
				m_UI.m_bShowHelp = !m_UI.m_bShowHelp; break;
			case VK_UP:
				break;
			case VK_DOWN:
				break;
			case VK_LEFT:
				break;
			case VK_RIGHT:
				break;

			}
		}

}

//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK DX10Base::OnDeviceRemoved(  )
{
    return true;
}

//--------------------------------------------------------------------------------------
// Update the MSAA sample count combo box for this format
//--------------------------------------------------------------------------------------
void DX10Base::GetMaxMSAASampleCount( ID3D10Device* pd3dDevice, DXGI_FORMAT fmt )
{

	UINT  iHighestSampleCount = 1;
	for( UINT i = 1; i <= D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT; i++ )
	{
		UINT Quality;
		if( SUCCEEDED( pd3dDevice->CheckMultisampleQualityLevels( fmt, i, &Quality ) ) &&
			Quality > 0 )
		{
			iHighestSampleCount = i;
		}
	}
	m_MSAASampleCount = iHighestSampleCount;


}
