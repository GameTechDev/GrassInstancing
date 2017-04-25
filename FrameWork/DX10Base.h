/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#define ALT_CAMERA

#pragma once
#include "GrassUI.h"
#include "DXUT.h"
#include "DXUTcamera.h"
#include "SDKmisc.h"
#include "CModel.h"
#ifdef ALT_CAMERA
	#include "camera.h"
#endif
#include "skybox.h"


#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

class DX10Base
{
protected:
	BOOL						m_bWireFrame;
	BOOL						m_bCleanUp;				// order of destruction is dependant on how the class is declared we use this bool to make sure state is cleared 
	GrassUI						m_UI;
#ifdef ALT_CAMERA
 	struct mouse
    {
	    int x;
	    int y;
	    int start_x;
	    int start_y;
	    bool Ldragging; // left button is dragging
	    bool Rdragging;	// etc
	    bool Mdragging;	// etc
    };

	camera*			            m_pCamera;               // A model viewing camera
    mouse					    m_Mouse;
#else
	CModelViewerCamera*			m_pCamera;               // A model viewing camera
#endif // ALT_CAMERA

	// D3DX effect stuff
	ID3D10Effect*							m_pEffect;				
	ID3D10EffectMatrixVariable*				m_pfxWorld;
	ID3D10EffectMatrixVariable*				m_pfxView;
	ID3D10EffectMatrixVariable*				m_pfxProj;
	ID3D10EffectScalarVariable*				m_fTimeVar;
	ID3D10EffectShaderResourceVariable*		m_ptxDiffuse;

	// Vars tied to effects
	D3DXMATRIX								m_World;
	D3DXMATRIX								m_View;
	D3DXMATRIX								m_Projection;
	float									m_fTime;

	
	unsigned int							m_MSAASampleCount;
	Skybox*									pSkybox;
public:

	DX10Base () : m_pEffect(0), m_bCleanUp( FALSE ), m_bWireFrame( FALSE ){ };
	
	virtual ~DX10Base();

	int Run();

	virtual void InitApp();
	
	// Query for MSAA support
	void  GetMaxMSAASampleCount( ID3D10Device* pd3dDevice, DXGI_FORMAT fmt );

	// Messaging Events
	virtual LRESULT CALLBACK MsgProc			 ( HWND hWnd, UINT uMsg, WPARAM wParam, 
												   LPARAM lParam, bool* pbNoFurtherProcessing );
	
	virtual void    CALLBACK KeyboardProc		 ( UINT nChar, bool bKeyDown, bool bAltDown );
	
	virtual void    CALLBACK OnGUIEvent			 ( UINT nEvent, int nControlID, CDXUTControl* pControl, 
												   void* pUserContext  );


	// Device Creation & Deletion
	virtual bool	CALLBACK IsDeviceAcceptable	 ( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, 
												   DXGI_FORMAT BackBufferFormat, bool bWindowed );
	
	virtual HRESULT CALLBACK OnCreateDevice		 ( ID3D10Device* pd3dDevice, 
												   const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );

	virtual bool	CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings );

	virtual bool	CALLBACK OnDeviceRemoved	 ( );

	virtual void    CALLBACK OnDestroyDevice	 ( );


	// DX10 Specific

	virtual void    CALLBACK OnSwapChainReleasing(  );

	virtual HRESULT CALLBACK OnSwapChainResized	 ( ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain, 
												   const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );


	// FrameUpdate

	virtual void    CALLBACK OnFrameMove		 ( double fTime, float fElapsedTime );

	virtual void    CALLBACK OnFrameRender       ( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime );


};


