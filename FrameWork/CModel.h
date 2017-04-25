/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CMODEL_
#define _CMODEL_

#include "SDKmesh.h"
#include "SDKmesh_old.h"

//#pragma warning ( disable : 4995 ) //get rid of the anoying deprecation warnings
//#include <string>

//#define SDKMESH

// Define the input layout
const D3D10_INPUT_ELEMENT_DESC default_layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};
const unsigned int numDefaultElements = sizeof(default_layout)/sizeof(D3D10_INPUT_ELEMENT_DESC);

template <class T>
class CModel
{

public:
	WCHAR* fname;
	DWORD id;

	T				m_mesh;
//	CDXUTMesh10		m_mesh;
//	CDXUTSDKMesh    m_mesh;
	D3DXVECTOR3		m_bvCenter;
	float			m_bvRadius;
	D3DXMATRIX	m_world;
	//CBoundingVolume bbox;
	ID3D10InputLayout* m_pVertexLayout;

	CModel():fname(NULL){};

	~CModel() { Destroy(); }

	D3DXMATRIX*	GetMatrix	( );
	void		SetMatrix	( ){};
	HRESULT		LoadFromFile( ID3D10Device* pd3dDevice, WCHAR* fileName, ID3D10EffectTechnique* pTechnique, 
								  const D3D10_INPUT_ELEMENT_DESC* pLayout = default_layout, 
								  UINT numElements = numDefaultElements  )
	{
		HRESULT hr;
		// Load the mesh from the specified file
		if(fname)
		{
			Destroy();
		}
		fname = new WCHAR[MAX_PATH];
		StringCchCopyW(fname, MAX_PATH,fileName);
	
		// Create the input layout
		D3D10_PASS_DESC PassDesc;
		pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
		V_RETURN( pd3dDevice->CreateInputLayout( pLayout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pVertexLayout ) );
		// Set the input layout
		pd3dDevice->IASetInputLayout( m_pVertexLayout );
// ANU TO DO toggle based on extension or specialazation
//		std::wstring str(fname);
		V_RETURN( m_mesh.Create( pd3dDevice,fname, (D3D10_INPUT_ELEMENT_DESC*)pLayout, numElements, true ));
//		V_RETURN( m_mesh.Create( pd3dDevice, fname, true ) );	

		return S_OK;
	}
	void Render	( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechnique, ID3D10EffectShaderResourceVariable* ptxDiffuseVariable )
	{
		pd3dDevice->IASetInputLayout( m_pVertexLayout );
		m_mesh.Render( pd3dDevice, pTechnique, ptxDiffuseVariable );
	}
	
	void		Destroy		( )
	{
		delete(fname);
		fname = NULL;
		SAFE_RELEASE( m_pVertexLayout );
		m_mesh.Destroy(); 
	};
	//HRESULT GenerateSphere(LPDIRECT3DDEVICE9 pd3dDevice, int radius, int slices, int stacks);
	// GetBoundingBox();
};

HRESULT	CModel<CDXUTSDKMesh>::LoadFromFile( ID3D10Device* pd3dDevice, WCHAR* fileName, ID3D10EffectTechnique* pTechnique, 
										   const D3D10_INPUT_ELEMENT_DESC* pLayout, UINT numElements )
{

	HRESULT hr;
	// Load the mesh from the specified file
	if(fname)
	{
		Destroy();
	}
	fname = new WCHAR[MAX_PATH];
	StringCchCopyW(fname, MAX_PATH,fileName);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	V_RETURN( pd3dDevice->CreateInputLayout( pLayout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pVertexLayout ) );

	// Set the input layout
	pd3dDevice->IASetInputLayout( m_pVertexLayout );
	// Load the mesh
	V_RETURN( m_mesh.Create( pd3dDevice, fname, true ) );	
	return S_OK;
}

#endif