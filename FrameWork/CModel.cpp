/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmesh.h"

#include "CModel.h"

/*
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

template <class T>
void CModel<T>::Destroy()
{
	delete(fname);
	fname = NULL;
	SAFE_RELEASE( m_pVertexLayout );
    m_mesh.Destroy(); 

}
*/
//m_mesh.Render( pd3dDevice, g_pTechnique, g_ptxDiffuseVariable );

//template <class T>
//void CModel::Render( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechnique, ID3D10EffectShaderResourceVariable* ptxDiffuseVariable )
//{
//	pd3dDevice->IASetInputLayout( m_pVertexLayout );
//	m_mesh.Render( pd3dDevice, pTechnique, ptxDiffuseVariable );
/*
	#ifdef SDKMESH	
	//	aModel.Render( pd3dDevice, g_pTechnique );
	//
	// Render the mesh
	//
	UINT Strides[1];
	UINT Offsets[1];
	ID3D10Buffer* pVB[1];
	pVB[0] = m_mesh.GetVB10(0,0);
	Strides[0] = (UINT)m_mesh.GetVertexStride(0,0);
	Offsets[0] = 0;
	pd3dDevice->IASetVertexBuffers( 0, 1, pVB, Strides, Offsets );
	pd3dDevice->IASetIndexBuffer( m_mesh.GetIB10(0), m_mesh.GetIBFormat10(0), 0 );

	D3D10_TECHNIQUE_DESC techDesc;
	pTechnique->GetDesc( &techDesc );
	SDKMESH_SUBSET* pSubset = NULL;
	ID3D10ShaderResourceView* pDiffuseRV = NULL;
	D3D10_PRIMITIVE_TOPOLOGY PrimType;

	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		for( UINT subset = 0; subset < m_mesh.GetNumSubsets(0); ++subset )
		{
			pSubset = m_mesh.GetSubset( 0,subset );

			PrimType = m_mesh.GetPrimitiveType10( (SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType );
			pd3dDevice->IASetPrimitiveTopology( PrimType );

			pDiffuseRV = m_mesh.GetMaterial(pSubset->MaterialID)->pDiffuseRV10;
			ptxDiffuseVariable->SetResource( pDiffuseRV );

			pTechnique->GetPassByIndex( p )->Apply(0);
			pd3dDevice->DrawIndexed( (UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart );
		}
	}
	#endif
*/
//}
