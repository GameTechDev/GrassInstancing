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

template <class T_VERTEX, class T_INSTANCE >
class InstancedBillboard
{

	int									vertexCnt;
	T_VERTEX*							pVertices;
	ID3D10Buffer*                       pVertexBuffer;

	D3D10_INPUT_ELEMENT_DESC*			pLayout;
	int									numElements;
	ID3D10InputLayout*                  pVertexLayout;

	TCHAR								pTextureName[MAX_PATH];
	ID3D10ShaderResourceView*           pTextureRV;
	ID3D10EffectTechnique*              pRenderTechnique;

	int									instanceCnt;
	int									maxInstanceCnt;
	T_INSTANCE*							pInstanceData;
	ID3D10Buffer*						pInstanceBuffer;

	HRESULT			LoadInstData ( ID3D10Device* pd3dDevice, ID3D10Buffer** ppInstanceData, 
								   void* pInData, int dataSize );

public:

	virtual void	Init		 ( TCHAR* texName, T_INSTANCE* pInInstanceData, int cnt, T_VERTEX* pInVertices, int numVerts, 
								   D3D10_INPUT_ELEMENT_DESC* layout, int numLayoutElements, 
								   WORD* indices = 0, int numIndices = 0 );

	virtual void	Destroy		 ( ){ };

	virtual HRESULT CreateDevice ( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechique );

	virtual void	DestroyDevice( );
//	virtual void    PreRender	 ( double fTime, float fElapsedTime ){};

	virtual void	Render		 ( ID3D10Device* pd3dDevice, ID3D10EffectShaderResourceVariable* pDiffuseVariable );
	
	virtual void	SetInstData	 ( T_INSTANCE* pInData, int numInstances );

};

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::SetInstData( T_INSTANCE* pInData, int numInstances )
{
	// ASSERT( numInstances > maxInstanceCnt );
	instanceCnt		= numInstances;
	void* pData		= NULL;
	pInstanceBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pData );
	memcpy( pData, (void*)pInData, sizeof(T_INSTANCE)*instanceCnt );
	pInstanceBuffer->Unmap();
}


template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::Init( TCHAR* texName, T_INSTANCE* pInInstanceData, int cnt , T_VERTEX* pInVertices, int numVerts, 
													D3D10_INPUT_ELEMENT_DESC* layout, int numLayoutElements, 
													WORD* indices /*= 0*/, int numIndices /*= 0 */ )
{
	_tcscpy(pTextureName, texName);

	pVertices		 =  pInVertices;
	vertexCnt		 =  numVerts;
	pLayout			 =  layout;
	numElements		 =  numLayoutElements;
	pInstanceData	 =  pInInstanceData;
	instanceCnt		 =  cnt;
	maxInstanceCnt	 =  cnt;
}

template <class T_VERTEX, class T_INSTANCE>
HRESULT InstancedBillboard<T_VERTEX, T_INSTANCE>::CreateDevice( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechique)
{
	HRESULT hr = S_OK;
	pRenderTechnique = pTechique;
	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	pRenderTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	hr = pd3dDevice->CreateInputLayout( pLayout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &pVertexLayout );

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

	// Create Instance Buffer
	// We're creating this buffer as dynamic because in a game, the instance data could be dynamic... aka
	// we could have moving trees.
	D3D10_BUFFER_DESC bufferDesc =
	{
		sizeof( T_INSTANCE )*instanceCnt,
		D3D10_USAGE_DYNAMIC,
		D3D10_BIND_VERTEX_BUFFER,
		D3D10_CPU_ACCESS_WRITE,
		0
	};

	V_RETURN( pd3dDevice->CreateBuffer( &bufferDesc, NULL, &pInstanceBuffer ) );


	// Load the Texture
	V_RETURN( D3DX10CreateShaderResourceViewFromFile( pd3dDevice, pTextureName, NULL, NULL, &pTextureRV, NULL ));

	//Initialize instance buffer with data passed in
	SetInstData( pInstanceData, instanceCnt ); 

	return hr;

}

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::DestroyDevice()
{
	SAFE_RELEASE( pVertexBuffer );
	SAFE_RELEASE( pVertexLayout );
	SAFE_RELEASE( pTextureRV );
	SAFE_RELEASE( pInstanceBuffer );
}

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::Render (ID3D10Device* pd3dDevice, ID3D10EffectShaderResourceVariable* pDiffuseVariable )
{
	// Set the input layout
	pd3dDevice->IASetInputLayout( pVertexLayout );

	ID3D10Buffer* pVB[2];
	UINT strides[2];
	UINT offsets[2] = {0,0};
	pVB[0] = pVertexBuffer;
	pVB[1] = pInstanceBuffer;
	strides[0] = sizeof ( T_VERTEX   );
	strides[1] = sizeof ( T_INSTANCE );
	pd3dDevice->IASetVertexBuffers( 0,       //first input slot for binding
									2,       //number of buffers in the array
									pVB,     //array of three vertex buffers
									strides, //array of stride values, one for each buffer
									offsets	 //array of offset values, one for each buffer
									);

	// Set primitive topology
	pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pDiffuseVariable->SetResource( pTextureRV );

	// Render Instanced Billboards
	D3D10_TECHNIQUE_DESC techDesc;
	pRenderTechnique->GetDesc( &techDesc );

	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pRenderTechnique->GetPassByIndex( p )->Apply( 0 );
		pd3dDevice->DrawInstanced( vertexCnt,    	// number of vertices per instance
					  instanceCnt,	 // number of instances
					  0,		 // Index of the first vertex
					  0		 // Index of the first instance
					  );
	}
}

template <class T_VERTEX, class T_INSTANCE>
HRESULT InstancedBillboard<T_VERTEX, T_INSTANCE>::LoadInstData( ID3D10Device* pd3dDevice, ID3D10Buffer** ppInstanceData, void* pInData, int dataSize )
{


	HRESULT hr = S_OK;

	// Create a resource with the input matrices
	// We're creating this buffer as dynamic because in a game, the instance data could be dynamic
	D3D10_BUFFER_DESC bufferDesc =
	{
		dataSize,
		D3D10_USAGE_DYNAMIC,
		D3D10_BIND_VERTEX_BUFFER,
		D3D10_CPU_ACCESS_WRITE,
		0
	};

	V_RETURN( pd3dDevice->CreateBuffer( &bufferDesc, NULL, ppInstanceData ) );

	void* pData = NULL;
	( *ppInstanceData )->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pData );

	memcpy( pData, pInData, dataSize );

	( *ppInstanceData )->Unmap();
	return hr;
}

