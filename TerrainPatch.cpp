/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "TerrainPatch.h"


TerrainPatch::TerrainPatch()
{
	g_pVertexBuffer  = NULL;
	pRenderTechnique = NULL;
	m_pGroundVerts   = 0;

}

HRESULT TerrainPatch::CreateDevice( ID3D10Device* pd3dDevice, ID3D10Effect* pEffect)
{
	HRESULT hr = S_OK;
	
	pRenderTechnique = pEffect->GetTechniqueByName( "Render" );
	if( m_pGroundVerts )
		__super::Init(L"texture//seafloor.dds", m_pGroundVerts, 6);
	else 
		__super::Init(L"texture//seafloor.dds", g_DefaultGroundVerts, 6);

	__super::CreateDevice(pd3dDevice , pRenderTechnique );
	
	return hr;

}

void TerrainPatch::DestroyDevice()
{
	__super::DestroyDevice();
	SAFE_RELEASE( g_pVertexBuffer );

}

void TerrainPatch::Generate( D3DXVECTOR3 center, float r, float texRepeat )
{
	m_pGroundVerts = new TerrainVertex[6];
	//D3DXVECTOR3(-60.0f,  -10.0f, 60.0f)
	m_pGroundVerts[0].Pos = D3DXVECTOR3( center.x - r, center.y, center.z + r );  
	m_pGroundVerts[0].Tex = D3DXVECTOR2( texRepeat, texRepeat );
//	{D3DXVECTOR3( 60.0f,  -10.0f, 60.0f), D3DXVECTOR2(0.0f, 10.0f )},
	m_pGroundVerts[1].Pos = D3DXVECTOR3( center.x + r, center.y, center.z + r );  
	m_pGroundVerts[1].Tex = D3DXVECTOR2(0, texRepeat );
//	{D3DXVECTOR3( 60.0f,   -10.0f, -60.0f), D3DXVECTOR2(0.0f, 0.0f )},
	m_pGroundVerts[2].Pos = D3DXVECTOR3( center.x + r, center.y, center.z - r );  
	m_pGroundVerts[2].Tex = D3DXVECTOR2(0, 0 );

//	{D3DXVECTOR3(-60.0f,  -10.0f, 60.0f), D3DXVECTOR2(10.0f, 10.0f )},
	m_pGroundVerts[3].Pos = D3DXVECTOR3( center.x - r, center.y, center.z + r );  
	m_pGroundVerts[3].Tex = D3DXVECTOR2(texRepeat, texRepeat );
//	{D3DXVECTOR3( 60.0f,  -10.0f, -60.0f), D3DXVECTOR2(0.0f, 0.0f )},
	m_pGroundVerts[4].Pos = D3DXVECTOR3( center.x + r, center.y, center.z - r );  
	m_pGroundVerts[4].Tex = D3DXVECTOR2(0, 0 );
//	{D3DXVECTOR3(-60.0f, -10.0f, -60.0f), D3DXVECTOR2(10.0f, 0.0f )},
	m_pGroundVerts[5].Pos = D3DXVECTOR3( center.x - r, center.y, center.z - r );  
	m_pGroundVerts[5].Tex = D3DXVECTOR2(texRepeat, 0.0 );

}

TerrainPatch::~TerrainPatch()
{
	SAFE_DELETE_ARRAY( m_pGroundVerts );
}