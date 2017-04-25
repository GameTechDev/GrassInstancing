/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "Grass.h"

HRESULT CALLBACK Grass::OnCreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc )
{
	
	HRESULT hr = __super::OnCreateDevice( pd3dDevice, pBackBufferSurfaceDesc );
	D3DXVECTOR3 terrainCenter = D3DXVECTOR3(0,-10,0);
	float terrainRadius = 45.0;
	float terrainTexRepeats = 10.0;
	terrain.Generate( terrainCenter,terrainRadius,terrainTexRepeats ); // Center, radius, texRepeats
	terrain.CreateDevice ( pd3dDevice, m_pEffect );
	float patchRadius = 15.0;
	unsigned int  grassBladesPerPatch = 70;
	bbGrassPatch.Generate( terrainCenter, terrainRadius, grassBladesPerPatch, patchRadius );
	bbGrassPatch.CreateDevice( pd3dDevice, m_pEffect );
	D3DXVECTOR2 treeCenter = D3DXVECTOR2(0, -25);
	trees.Initialize( treeCenter );
	trees.CreateDevice( pd3dDevice, m_pEffect );

	return hr;
}

void    CALLBACK Grass::OnFrameMove( double fTime, float fElapsedTime )
{
	__super::OnFrameMove(fTime, fElapsedTime);
	
	D3DXVECTOR3 eyePt    = *m_pCamera->GetEyePt();
	D3DXVECTOR3 lookAtPt = *m_pCamera->GetLookAtPt();
	trees.OrientTreesTo(lookAtPt - eyePt);
	// Cull geometry not visible
	bbGrassPatch.Cull( D3DX_PI/8 /* frustum cone angle = camera FOV/2*/, eyePt, lookAtPt);


}

void    CALLBACK Grass::OnFrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime )
{
	__super::OnFrameRender( pd3dDevice, fTime, fElapsedTime );
	terrain.Render( pd3dDevice, m_ptxDiffuse );
	trees.Render(  pd3dDevice, m_ptxDiffuse );
	bbGrassPatch.Render( pd3dDevice, m_ptxDiffuse );
	m_UI.Render(fElapsedTime); // No UI by default

}

void    CALLBACK Grass::OnDestroyDevice()
{
	terrain.DestroyDevice();
	bbGrassPatch.DestroyDevice();
	trees.DestroyDevice();
	trees.Destroy();
	__super::OnDestroyDevice( );

}

