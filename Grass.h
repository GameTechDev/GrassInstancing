/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GrassUI.h"
#include "Framework/DX10Base.h"
#include "TerrainPatch.h"
#include "BillboardedTrees.h"
#include "BBGrassPatch.h"
class Grass:public DX10Base
{

	BBGrassPatch							bbGrassPatch;
	TerrainPatch							terrain;
	BillboardedTrees						trees;

public:
	void InitApp()
	{
		// call parent class
		__super::InitApp();
	}	
	// FrameUpdate

	HRESULT CALLBACK OnCreateDevice		 ( ID3D10Device* pd3dDevice, 
										   const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );
	void    CALLBACK OnDestroyDevice	 ( );
	
	void    CALLBACK OnFrameMove		 ( double fTime, float fElapsedTime );

	void    CALLBACK OnFrameRender       ( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime );


};
