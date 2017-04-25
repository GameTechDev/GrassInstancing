/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class DX10BaseHelper
{
public:
	static bool CALLBACK IsDeviceAcceptable(
		UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
		DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext )
	{
		return reinterpret_cast<DX10Base*>(pUserContext)
			->IsDeviceAcceptable(Adapter,Output,DeviceType,BufferFormat,bWindowed);
	}

	static HRESULT CALLBACK OnCreateDevice(
		ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC*
		in_pBufferSurfaceDesc, void* pUserContext )
	{
		return reinterpret_cast<DX10Base*>(pUserContext)
			->OnCreateDevice(pd3dDevice, in_pBufferSurfaceDesc);
	}

	static bool CALLBACK ModifyDeviceSettings(
		DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
	{
		return reinterpret_cast<DX10Base*>(pUserContext)
			->ModifyDeviceSettings(pDeviceSettings);
	}

	static HRESULT CALLBACK OnResizedSwapChain(
		ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain,
		const DXGI_SURFACE_DESC* in_pBufferSurfaceDesc, void* pUserContext )
	{
		return reinterpret_cast<DX10Base*>(pUserContext)
			->OnSwapChainResized(pd3dDevice, pSwapChain, in_pBufferSurfaceDesc);
	}

	static void CALLBACK OnReleasingSwapChain( void* pUserContext )
	{
		reinterpret_cast<DX10Base*>(pUserContext)->OnSwapChainReleasing();
	}


	static bool CALLBACK OnDeviceRemoved(void* pUserContext)
	{
		return reinterpret_cast<DX10Base*>(pUserContext)->OnDeviceRemoved();
	}

	static void CALLBACK OnDestroyDevice( void* pUserContext )
	{
		reinterpret_cast<DX10Base*>(pUserContext)->OnDestroyDevice();
	}


	// App Specific 
	static void CALLBACK OnFrameMove(
		double fTime, float fElapsedTime, void* pUserContext)
	{
		reinterpret_cast<DX10Base*>(pUserContext)->OnFrameMove(fTime, fElapsedTime);
	}

	static void CALLBACK OnFrameRender(
		ID3D10Device* pd3dDevice,double fTime,float fElapsedTime,void* pUserContext)
	{
		return reinterpret_cast<DX10Base*>(pUserContext)
			->OnFrameRender(pd3dDevice, fTime, fElapsedTime);
	}


	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext )
	{
		return reinterpret_cast<DX10Base*>(pUserContext)
			->MsgProc(hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing);
	}



	// UI Events
	static void CALLBACK KeyboardProc(
		UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
	{
		reinterpret_cast<DX10Base*>(pUserContext)->KeyboardProc( nChar, bKeyDown, bAltDown );
	}

};

