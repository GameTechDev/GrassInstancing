/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////


#include "DXUT.h"
#include "BBGrassPatch.h"

BOOL SphereConeTest ( D3DXVECTOR3 sphereCenter, float radius, float fov, D3DXVECTOR3 eyePt, D3DXVECTOR3 lookAt);

static const D3D10_INPUT_ELEMENT_DESC grassLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA,   0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA,   0 },
	{ "vPPos",    0, DXGI_FORMAT_R32G32_FLOAT,	  1, 0,	 D3D10_INPUT_PER_INSTANCE_DATA, 1 },

};

static const UINT numGrassLayoutElements = sizeof( grassLayout ) / sizeof( grassLayout[0] );


BBGrassPatch::BBGrassPatch( int numBB, int inPatchCount, D3DXVECTOR2* inPatchPos )
{

	m_numGrassBB = numBB;
	patchPos     = inPatchPos;
	patchCount	 = inPatchCount;
}
 
BBGrassPatch::~BBGrassPatch( )
{
	SAFE_DELETE_ARRAY ( patchPos );
	SAFE_DELETE_ARRAY ( visiblePatches );
	SAFE_DELETE_ARRAY ( m_grassVerts );
}

HRESULT BBGrassPatch::CreateDevice( ID3D10Device* pd3dDevice, ID3D10Effect* pEffect )
{
	HRESULT hr = S_OK;
	__super::Init(L"texture//grassY.dds", patchPos, patchCount, 
					m_grassVerts, 6*m_numGrassBB,(D3D10_INPUT_ELEMENT_DESC*)grassLayout, numGrassLayoutElements );
	ID3D10EffectTechnique*    pRenderTechnique;
	pRenderTechnique = pEffect->GetTechniqueByName( "RenderGrass" );
	hr = __super::CreateDevice( pd3dDevice , pRenderTechnique );

//__super::SetInstData( patchPos, patchCount );
	return hr;

}


void BBGrassPatch::DestroyDevice()
{
	InstancedBillboard::DestroyDevice();

}


void BBGrassPatch::GeneratePatch( unsigned int bladeCnt, float scale )
{
	m_numGrassBB = bladeCnt;
	m_grassVerts = new BBGrassVertex[m_numGrassBB*6];

	D3DXMATRIX mat;
	D3DXMATRIX matRandom;
	D3DXMatrixIdentity( &mat);

	for( int j=0; j< m_numGrassBB; j++ )
	{
		// Generate each quad at random position, orientation, height
		D3DXMatrixIdentity( &matRandom);
	//	float scale = 10.0f;
		float angle = ((float)rand()/RAND_MAX - 0.5f)*2 * D3DX_PI; // angle = [-pi,pi]
		float dx = ((float)rand()/RAND_MAX - 0.5f)*2 * scale;
		float dz = ((float)rand()/RAND_MAX - 0.5f)*2 * scale;
		float heightScale = ((float)rand()/RAND_MAX - 0.5f) / 2.0f + 1.0f;

		D3DXMatrixRotationY( &mat, angle);
		D3DXMatrixMultiply( &matRandom, &matRandom, &mat );
		D3DXMatrixTranslation( &mat, dx, 0.0f, dz);			
		D3DXMatrixMultiply( &matRandom, &matRandom, &mat );
		D3DXMatrixScaling( &mat, 1.0f, 1.0f, heightScale);			
		D3DXMatrixMultiply( &matRandom, &matRandom, &mat );

		// Apply the transformation to each vertex of the quad
		for( DWORD i=0; i< 6; i++ )
		{
			D3DXVECTOR4 pos, outPos;
			pos.x = grassBBVerts[i].Pos.x;
			pos.y = grassBBVerts[i].Pos.y;
			pos.z = grassBBVerts[i].Pos.z;
			pos.w = 1.0;
			D3DXVec3Transform(&outPos, (D3DXVECTOR3 *)&pos, (D3DXMATRIX *)&matRandom);
			int index = j * 6 + i;
			m_grassVerts[index].Pos.x = outPos.x;
			m_grassVerts[index].Pos.y = outPos.y;
			m_grassVerts[index].Pos.z = outPos.z;
			m_grassVerts[index].Tex = grassBBVerts[i].Tex;
		}
	}

}

void BBGrassPatch::Cull( float fov, D3DXVECTOR3 eyePt, D3DXVECTOR3 lookAt )
{
	int numVisibleInstances = 0;
	D3DXVECTOR3 patchCenter3D;
	BOOL visible;
	for( unsigned int i = 0 ; i < patchCount; i++)
	{	
		patchCenter3D.x = patchPos[i].x; 	
		patchCenter3D.z = patchPos[i].y; 	
		patchCenter3D.y = 0; // we are only storing x,z positions for the patches
		// we want to cull the patch with a sphere of radius = to diagonal of square patch 
		// hence radiusPatchBoundingSphere = 1.414*rpatch
		visible = SphereConeTest ( patchCenter3D, patchRadius*1.414f, fov, eyePt, lookAt);
		if( visible )
		{
			// visible add to draw list 
			visiblePatches[numVisibleInstances] = patchPos[i];
			numVisibleInstances++;
		}
	}

	SetInstData( visiblePatches, numVisibleInstances );
}


void BBGrassPatch::Generate( D3DXVECTOR3 surfaceCenter, float surfaceR, unsigned int patchBladeCnt, float inPatchRadius )
{
	patchRadius     = inPatchRadius;
	GeneratePatch( patchBladeCnt, patchRadius );

	patchCountX		= int( surfaceR/patchRadius );
	patchCountZ		= int( surfaceR/patchRadius );
	patchCount		= (2*patchCountX-1)*(2*patchCountZ-1);
	patchPos		= new D3DXVECTOR2[patchCount];
	visiblePatches  = new D3DXVECTOR2[patchCount];
	int k = 0;
	for(int i = -(patchCountX-1); i < patchCountX; i++)
	{
		for(int j = -(patchCountZ-1); j < patchCountZ; j++)
		{
			patchPos[k].x = surfaceCenter.x + i*patchRadius;
			patchPos[k].y = surfaceCenter.z + j*patchRadius;
			k++;
		}

	}
}