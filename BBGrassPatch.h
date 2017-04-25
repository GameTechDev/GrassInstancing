/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <d3d10.h>
#include <d3dx10.h>
#define DEFAULT_NUM_GRASS_BB 70
#include "InstancedBillboard.h"

struct BBGrassVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

//  geometry for single grass blade 
static BBGrassVertex grassBBVerts[] =
{
	//					x     y      z					 tu1   tv1    
	{  D3DXVECTOR3( -10.0f,   0.0f,   0.0f),  D3DXVECTOR2( 1.0f,  0.01f )},
	{  D3DXVECTOR3(  10.0f,   0.0f,   0.0f),  D3DXVECTOR2( 0.01f, 0.01f )},
	{  D3DXVECTOR3(  10.0f,  -10.0f,  0.0f),  D3DXVECTOR2( 0.01f, 1.0f ) },

	{  D3DXVECTOR3(  -10.0f,   0.0f,  0.0f),  D3DXVECTOR2( 1.0f,  0.01f )},
	{  D3DXVECTOR3(   10.0f,  -10.0f, 0.0f),  D3DXVECTOR2( 0.01f, 1.0f ) },
	{  D3DXVECTOR3(  -10.0f,  -10.0f, 0.0f),  D3DXVECTOR2( 1.0f,  1.0f ) }, 
};

//  x,z instance positions ...
static D3DXVECTOR2 defaultPatchPos[] =
{
	//			   x      z					
	D3DXVECTOR2(  20.0f, 0.0f ) ,
	D3DXVECTOR2(  0.0f,  0.0f ) ,
	D3DXVECTOR2( -20.0f, 0.0f ) ,

	D3DXVECTOR2(  20.0f, -20.0f ) ,
	D3DXVECTOR2(  0.0f,  -20.0f  ) ,
	D3DXVECTOR2( -20.0f, -20.0f ) ,

	D3DXVECTOR2(  20.0f,  40.0f ) ,
	D3DXVECTOR2(  20.0f,  40.0f ) ,
	D3DXVECTOR2(  0.0f,   40.0f ) ,
	D3DXVECTOR2( -20.0f,  40.0f ),
	D3DXVECTOR2( -40.0,	  40.0f ),

	D3DXVECTOR2(  40.0f,  20.0f ) ,
	D3DXVECTOR2(  20.0f,  20.0f ) ,
	D3DXVECTOR2(  0.0f,   20.0f ) ,
	D3DXVECTOR2( -20.0f,  20.0f ) ,
	D3DXVECTOR2( -40.0f,  20.0f ) ,
};

const int defaultPatchCount = 16;

class BBGrassPatch : public InstancedBillboard <BBGrassVertex, D3DXVECTOR2> 
{
	int									m_numGrassBB;
	BBGrassVertex*						m_grassVerts;
	float								patchRadius;
	int									patchCountX;
	int									patchCountZ;
	unsigned int						patchCount;
	D3DXVECTOR2*						patchPos;
	D3DXVECTOR2*						visiblePatches;
	// Generates & Randomly Orients Blades in a patch
	void	GeneratePatch( unsigned int bladeCnt, float scale);

public:
	
	BBGrassPatch( int numBB = DEFAULT_NUM_GRASS_BB, int inPatchCount = defaultPatchCount, D3DXVECTOR2* inPatchPos = defaultPatchPos );
	~BBGrassPatch( );
	HRESULT CreateDevice ( ID3D10Device* m_pd3dDevice, ID3D10Effect* m_pEffect );
	void    Cull		 ( float fov, D3DXVECTOR3 eyePt, D3DXVECTOR3 lookAt );
	void	DestroyDevice( );
	void	Generate	 ( D3DXVECTOR3 surfaceCenter, float surfaceR, unsigned int patchBladeCnt, float inPatchRadius );
	
};