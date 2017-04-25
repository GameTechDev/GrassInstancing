/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning ( disable : 4995 4996 )
#include <d3d10.h>
#include <d3dx10.h>
#include <string.h>

struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

// Define a default input layout
static const D3D10_INPUT_ELEMENT_DESC defaultlayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};
static const UINT defaultnumElements = sizeof( defaultlayout ) / sizeof( defaultlayout[0] );


template < class T_VERTEX >
class Billboard 
{
	int									vertexCnt;
	T_VERTEX*							pVertices;
	D3D10_INPUT_ELEMENT_DESC*			pLayout;
	int									numElements;
	ID3D10InputLayout*                  pVertexLayout;
	ID3D10Buffer*                       pVertexBuffer;
	TCHAR								pTextureName[MAX_PATH];
	ID3D10ShaderResourceView*           pTextureRV;
	ID3D10EffectTechnique*              pRenderTechnique;

public:
	Billboard( );
	virtual ~Billboard(){};

	virtual void	Init		 ( TCHAR* texName, T_VERTEX* pInVertices, int numVerts, D3D10_INPUT_ELEMENT_DESC* layout = (D3D10_INPUT_ELEMENT_DESC*)defaultlayout, int numLayoutElements = defaultnumElements, 
									WORD* indices = 0, int numIndices = 0 );
	virtual void	Destroy		 ( ) { };
	
	virtual HRESULT CreateDevice ( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechique );
	
	virtual void	DestroyDevice( );
	
	virtual void	Render		 ( ID3D10Device* pd3dDevice, ID3D10EffectShaderResourceVariable* pDiffuseVariable );

};

template < class T_VERTEX  >
HRESULT Billboard<T_VERTEX >
::CreateDevice( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechique )
{
	HRESULT hr		 = S_OK;
	pRenderTechnique = pTechique;

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	pRenderTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	hr = pd3dDevice->CreateInputLayout( pLayout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &pVertexLayout );
	V_RETURN( hr );

	// Initialize Vertex Buffers
	D3D10_BUFFER_DESC		bd;
	D3D10_SUBRESOURCE_DATA	InitData;

	bd.Usage			= D3D10_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( T_VERTEX ) * vertexCnt;
	bd.BindFlags		= D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	bd.MiscFlags		= 0;
	InitData.pSysMem	= pVertices;

	V_RETURN( pd3dDevice->CreateBuffer( &bd, &InitData, &pVertexBuffer ));

	// Load the Texture
	V_RETURN( D3DX10CreateShaderResourceViewFromFile( pd3dDevice, pTextureName, NULL, NULL, &pTextureRV, NULL ));

	return hr;

}

template < class T_VERTEX  >
Billboard<T_VERTEX >::Billboard()
{
	pVertexLayout	 = NULL;
	pVertexBuffer	 = NULL;
	pTextureRV		 = NULL;
	pRenderTechnique = NULL;

}

template < class T_VERTEX  >
void Billboard<T_VERTEX >
::Init( TCHAR* texName, T_VERTEX* pInVertices, int numVerts, D3D10_INPUT_ELEMENT_DESC* layout, int numLayoutElements, WORD* indices /*= 0*/, int numIndices /*= 0 */ )
{
	_tcscpy(pTextureName, texName);
	pVertices	= pInVertices;
	vertexCnt	= numVerts;
	pLayout		= layout;
	numElements = numLayoutElements;
}



template < class T_VERTEX  >
void Billboard<T_VERTEX >
::DestroyDevice()
{
	SAFE_RELEASE( pVertexBuffer );
	SAFE_RELEASE( pVertexLayout );
	SAFE_RELEASE( pTextureRV );
}
template < class T_VERTEX  >
void Billboard<T_VERTEX >
::Render( ID3D10Device* pd3dDevice, ID3D10EffectShaderResourceVariable* pDiffuseVariable )
{
	// Set diffuse texture 
	pDiffuseVariable->SetResource( pTextureRV );

	// Set Vertex Buffers & layout
	pd3dDevice->IASetInputLayout( pVertexLayout );

	unsigned int offset = 0,  stride = sizeof( SimpleVertex );
	pd3dDevice->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
	pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Render Billboard
	D3D10_TECHNIQUE_DESC techDesc;
	pRenderTechnique->GetDesc( &techDesc );

	for( unsigned int p = 0; p < techDesc.Passes; ++p )
	{
		pRenderTechnique->GetPassByIndex( p )->Apply( 0 );
		pd3dDevice->Draw( vertexCnt, 0 );
	}

}

