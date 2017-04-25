/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <d3d10.h>
#include <d3dx10.h>
#include "Billboard.h"

struct TerrainVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;

};

static TerrainVertex g_DefaultGroundVerts[]=
{
	//  x       y      z     tu1   tv1    
	{D3DXVECTOR3(-60.0f,  -10.0f, 60.0f), D3DXVECTOR2(10.0f, 10.0f )},
	{D3DXVECTOR3( 60.0f,  -10.0f, 60.0f), D3DXVECTOR2(0.0f, 10.0f )},
	{D3DXVECTOR3( 60.0f,   -10.0f, -60.0f), D3DXVECTOR2(0.0f, 0.0f )},

	{D3DXVECTOR3(-60.0f,  -10.0f, 60.0f), D3DXVECTOR2(10.0f, 10.0f )},
	{D3DXVECTOR3( 60.0f,  -10.0f, -60.0f), D3DXVECTOR2(0.0f, 0.0f )},
	{D3DXVECTOR3(-60.0f, -10.0f, -60.0f), D3DXVECTOR2(10.0f, 0.0f )},
};



class TerrainPatch : public Billboard <TerrainVertex>
{
	ID3D10Buffer*                       g_pVertexBuffer;
	ID3D10EffectTechnique*              pRenderTechnique;
	TerrainVertex*						m_pGroundVerts;
public:
	TerrainPatch  ( );
	~TerrainPatch ( );
	void	Generate	( D3DXVECTOR3 center, float radius, float texRepeat ); /* TODO - Extend to add height field*/
	HRESULT CreateDevice	( ID3D10Device* m_pd3dDevice, ID3D10Effect* m_pEffect );
	void	DestroyDevice	( );

};


