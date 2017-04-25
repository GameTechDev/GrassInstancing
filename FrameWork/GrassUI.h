/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTsettingsdlg.h"
#include "SDKmisc.h"

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------

#define IDC_TOGGLEFULLSCREEN          1
#define IDC_TOGGLEREF                 2
#define IDC_CHANGEDEVICE              3
#define IDC_TECHNIQUE                 4

class GrassUI
{
	CDXUTDialogResourceManager	m_DialogResourceManager; // manager for shared resources of dialogs
	CDXUTDialog					m_HUD;					 // dialog for standard controls
	CDXUTDialog					m_DefaultUI;              // dialog for sample specific controls
	ID3DX10Font*				m_pFont;				 // Font for drawing text
	ID3DX10Sprite*				m_pTextSprite;			 // Sprite for batching draw text calls
	int							m_backBufWidth;
	int							m_backBufHeight;
	CDXUTTextHelper*			m_pTxtHelper;
public:
	BOOL						m_bShowHelp;
	GrassUI():m_pFont(NULL), m_pTextSprite(NULL), m_bShowHelp(FALSE) {};
	
	~GrassUI() { Destroy(); }

	LRESULT Create(ID3D10Device* pd3dDevice);
	void Init();
	void Destroy();
	void Update();
	void Render(float fElapsedTime );
	void RenderText();
	void ToggleHelp(){m_bShowHelp = !m_bShowHelp;};
	BOOL MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

	void OnD3D10SwapChainReleasing();
	void OnD3D10SwapChainResized( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );

};

