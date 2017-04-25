/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "BillboardedTrees.h"

static const D3D10_INPUT_ELEMENT_DESC treeLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA,   0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA,   0 },
	{ "offPos",   0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,	 D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "scale",    0, DXGI_FORMAT_R32G32_FLOAT,	  1, 12, D3D10_INPUT_PER_INSTANCE_DATA, 1 },

};

static const UINT numTreeLayoutElements = sizeof( treeLayout ) / sizeof( treeLayout[0] );


HRESULT BillboardedTrees::Initialize( D3DXVECTOR2 offPos, int numTrees, float height, float width, float spread )
{
	m_treeCnt = numTrees;
	D3DXMatrixIdentity( &m_TreeOrientMatrix);
	float yPos = -10.0; // = pos.y
	m_pTreeVerts			= new SimpleVertex[6];
	m_pTreeData				= new TREE_INSTANCE_DATA[numTrees];

	m_pTreeVerts[0].Pos		= D3DXVECTOR3(-width,  2*height, 0 );
	m_pTreeVerts[1].Pos		= D3DXVECTOR3( width,  2*height, 0 );
	m_pTreeVerts[2].Pos		= D3DXVECTOR3( width,  0,		 0 );

	m_pTreeVerts[3].Pos		= D3DXVECTOR3(-width,  2*height, 0 );
	m_pTreeVerts[4].Pos		= D3DXVECTOR3( width,  0,		 0 );
	m_pTreeVerts[5].Pos		= D3DXVECTOR3(-width,  0,		 0 );

	m_pTreeVerts[0].Tex     =  D3DXVECTOR2( 1.0f,  0.01f );
	m_pTreeVerts[1].Tex		= D3DXVECTOR2( 0.0f,  0.01f );
	m_pTreeVerts[2].Tex		= D3DXVECTOR2( 0.01f, 1.0f );

	m_pTreeVerts[3].Tex		= D3DXVECTOR2( 1.0f,  0.01f );
	m_pTreeVerts[4].Tex		= D3DXVECTOR2( 0.0f,  1.0f );
	m_pTreeVerts[5].Tex		= D3DXVECTOR2( 1.0f,  1.0f );

	// randomly position trees
	
	for( int i=0; i<m_treeCnt; i++ )
	{

		float rot  = 2.0f*D3DX_PI*(float)rand()/RAND_MAX;
		float spreadRadius = spread + 2.0f*spread*(float)rand()/RAND_MAX;

		m_pTreeData[i].Pos.x	= spreadRadius * sinf(rot)+offPos.x;
		m_pTreeData[i].Pos.z    = spreadRadius * cosf(rot)+offPos.y;
		m_pTreeData[i].Pos.y    = yPos; 	//TODO HeightField( xPos, yPos );
		
		// Vary the size of the trees a bit randomly
		 m_pTreeData[i].scale.x  = (width  + 0.3f * (float)(rand()-rand())/RAND_MAX)/width;
		 m_pTreeData[i].scale.y  = (height + 0.5f * (float)(rand()-rand())/RAND_MAX)/height;
		
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: TreeSortCB()
// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------

HRESULT BillboardedTrees::CreateDevice(  ID3D10Device* pd3dDevice, ID3D10Effect* pEffect )
{

	HRESULT hr = S_OK;

	// Obtain the technique
	__super::Init(L"texture//tree02s.dds", m_pTreeData, m_treeCnt, 
					m_pTreeVerts, 6,(D3D10_INPUT_ELEMENT_DESC*)treeLayout, numTreeLayoutElements );

	ID3D10EffectTechnique*    pRenderTechnique;
	pRenderTechnique = pEffect->GetTechniqueByName( "RenderTree" );
	hr = __super::CreateDevice( pd3dDevice , pRenderTechnique );
	m_pfxOrientation = pEffect->GetVariableByName( "Orientation" )->AsMatrix();

	return hr;

}
void BillboardedTrees::DestroyDevice()
{

	__super::DestroyDevice();
	SAFE_DELETE_ARRAY( m_pTreeVerts );
	SAFE_DELETE_ARRAY( m_pTreeData  );
}

void BillboardedTrees::OrientTreesTo(D3DXVECTOR3 vDir)
{

	m_Orientation = vDir;

	if( vDir.x < 0.0f )
		D3DXMatrixRotationY( &m_TreeOrientMatrix, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
	else
		D3DXMatrixRotationY( &m_TreeOrientMatrix, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );

	//// Sort trees in back-to-front order
//	qsort( m_Trees, m_treeCnt, sizeof(Tree), TreeSortCB );

}
//-----------------------------------------------------------------------------
// Name: DrawTrees()
// Desc:
//-----------------------------------------------------------------------------
void BillboardedTrees::Render(ID3D10Device* pd3dDevice, ID3D10EffectShaderResourceVariable* pDiffuseVariable )
{
	m_pfxOrientation->SetMatrix((float*) &m_TreeOrientMatrix );
	__super::Render( pd3dDevice,pDiffuseVariable );

}

void BillboardedTrees::Destroy()
{
	SAFE_DELETE_ARRAY (m_pTreeVerts);
}
