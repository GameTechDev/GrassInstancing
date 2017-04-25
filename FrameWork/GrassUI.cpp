/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include <DXUT.h>
#include "GrassUI.h"

CD3DSettingsDlg				g_D3DSettingsDlg;          // Device settings dialog

LRESULT GrassUI::Create(ID3D10Device* pd3dDevice)
{
	HRESULT hr;
	V_RETURN( m_DialogResourceManager.OnD3D10CreateDevice( pd3dDevice ) );
	V_RETURN( g_D3DSettingsDlg.OnD3D10CreateDevice( pd3dDevice ) );
	V_RETURN( D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		L"Arial", &m_pFont ) );
	V_RETURN( D3DX10CreateSprite( pd3dDevice, 512, &m_pTextSprite ) );
	m_pTxtHelper = new CDXUTTextHelper( NULL, NULL, m_pFont, m_pTextSprite, 15 );
	return S_OK;

}

void GrassUI::Init()
{
//	HRESULT hr;
	// Initialize dialogs
	g_D3DSettingsDlg.Init( &m_DialogResourceManager );
	m_HUD.Init( &m_DialogResourceManager );
	m_DefaultUI.Init( &m_DialogResourceManager );

	m_HUD.SetCallback( OnGUIEvent ); int iX = 15; int iY = 10; 
	m_HUD.AddButton  ( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", iX, iY, 125, 22 );
	m_HUD.AddButton  ( IDC_TOGGLEREF,		 L"Toggle REF (F3)",    iX, iY += 24, 125, 22, VK_F3 );
	m_HUD.AddButton  ( IDC_CHANGEDEVICE,	 L"Change device (F2)", iX, iY += 24, 125, 22, VK_F2 );
	iX = 15;
	iY = 10;
	m_DefaultUI.SetCallback( OnGUIEvent );

	// Title font for static

}

void GrassUI::Destroy()
{
	m_DialogResourceManager.OnD3D10DestroyDevice();
	g_D3DSettingsDlg.OnD3D10DestroyDevice();
	SAFE_DELETE	( m_pTxtHelper );
	SAFE_RELEASE( m_pFont );
	SAFE_RELEASE( m_pTextSprite );

}

BOOL GrassUI::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	*pbNoFurtherProcessing = m_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return FALSE;

	if( g_D3DSettingsDlg.IsActive() )
	{
		g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
		return FALSE;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = m_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return TRUE;
	*pbNoFurtherProcessing = m_DefaultUI.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return TRUE;
	
	return FALSE;
}

void GrassUI::OnD3D10SwapChainReleasing( )
{
	m_DialogResourceManager.OnD3D10ReleasingSwapChain();
}
//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void GrassUI::RenderText()
{

	// The helper object simply helps keep track of text position, and color
	// and then it calls pFont->DrawText( m_pTextSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
	// If NULL is passed in as the sprite object, then it will work however the 
	// pFont->DrawText() will not be batched together.  Batching calls will improves performance.
	m_pTxtHelper->Begin();
	m_pTxtHelper->SetInsertionPos( 5, 5 );
	m_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	if( m_bShowHelp )
	{
		// Output statistics
		m_pTxtHelper->DrawTextLine( DXUTGetFrameStats(true) ); // Show FPS
		m_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

	
		m_pTxtHelper->DrawTextLine( L"Rotate Camera		  : Left Mouse Drag" );
		m_pTxtHelper->DrawTextLine( L"Move Camera up,down : Right Mouse Drag" );
		m_pTxtHelper->DrawTextLine( L"Move Camera		  : W, A, S, D " );
		m_pTxtHelper->DrawTextLine( L"Quit: ESC" );
	}
	else
	{
		m_pTxtHelper->DrawTextLine( L"Press F1 for menu & help" );
	}
	m_pTxtHelper->End();
}
void GrassUI::Render(float fElapsedTime)
{
	if(g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.OnRender( fElapsedTime );

	}
	else
	{
		if( m_bShowHelp )
		{
			m_HUD.OnRender( fElapsedTime  );
			m_DefaultUI.OnRender( fElapsedTime );
		}
		RenderText();
	}

}
void GrassUI::OnD3D10SwapChainResized( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc )
{
	m_DialogResourceManager.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc );
	g_D3DSettingsDlg.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc );

	m_backBufHeight = pBackBufferSurfaceDesc->Height;
	m_backBufWidth	= pBackBufferSurfaceDesc->Width;
	// Create a sprite to help batch calls when drawing many lines of text
	m_HUD.SetLocation( m_backBufWidth-170, 0 );
	m_HUD.SetSize( 170, 170 );
	m_DefaultUI.SetLocation(0, 0);
	m_DefaultUI.SetSize(m_backBufWidth, m_backBufHeight);

}
void CALLBACK GrassUI::OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext  )
{
	switch( nControlID )
	{
	case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
	case IDC_TOGGLEREF:        DXUTToggleREF(); break;
	case IDC_CHANGEDEVICE:     g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() ); break;

	}

}


