/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "Skybox.h"

Skybox::Skybox( ID3D10Device* pd3dDevice, float size )
{
	this->pd3dDevice = pd3dDevice;
	this->size = size;
	this->InitEffects();
	this->FillBuffers();
}

Skybox::~Skybox( void )
{
	SAFE_RELEASE( g_pSkyboxIndexBuffer );
	SAFE_RELEASE( g_pSkyboxVertexBuffer );
	SAFE_RELEASE( g_pSkyboxVLayout );
	SAFE_RELEASE( g_pSRVSkyboxCubemap );
	SAFE_RELEASE( g_pSkyboxEffect );
}


// Initialize Effects
HRESULT Skybox::InitEffects( void )
{
	HRESULT hr;

    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"fx/skyboxNEW.fx" ) );
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3D10_SHADER_DEBUG; // to debug shaders
#endif

	V_RETURN( D3DX10CreateEffectFromFile( str, NULL, NULL, "fx_4_0", dwShaderFlags, 0, pd3dDevice, NULL,
                                              NULL, &g_pSkyboxEffect, NULL, NULL ) );

    // Get effects variables
	g_pSkyboxTechnique = g_pSkyboxEffect->GetTechniqueByName( "RenderSkybox" );
	g_pSRVSkyboxCubemapVariable = g_pSkyboxEffect->GetVariableByName( "g_EnvironmentMap" )->AsShaderResource();
	g_pmViewProj = g_pSkyboxEffect->GetVariableByName( "g_mViewProjection" )->AsMatrix();
	g_pfSunAlpha = g_pSkyboxEffect->GetVariableByName( "g_SunAlpha" )->AsScalar();
	g_pfSunTheta = g_pSkyboxEffect->GetVariableByName( "g_SunTheta" )->AsScalar();
	g_pfSunShininess = g_pSkyboxEffect->GetVariableByName( "g_SunShininess" )->AsScalar();
	g_pfSunStrength = g_pSkyboxEffect->GetVariableByName( "g_SunStrength" )->AsScalar();

	// Set constant sun variables
	g_pfSunAlpha->SetFloat( 1.38f );
	g_pfSunTheta->SetFloat( 1.09f );
	g_pfSunShininess->SetFloat( 84.0f );
	g_pfSunStrength->SetFloat( 12.0f );

	// ...Including skybox texture
	V_RETURN( D3DX10CreateShaderResourceViewFromFile( pd3dDevice, L"texture//cubemap-evul.dds", NULL, NULL, &g_pSRVSkyboxCubemap, NULL ) );
	g_pSRVSkyboxCubemapVariable->SetResource( g_pSRVSkyboxCubemap );
	return S_OK;
}

// Fill Buffers
HRESULT Skybox::FillBuffers( void )
{
	HRESULT hr;

	// Define the input layout
    D3D10_INPUT_ELEMENT_DESC Skyboxlayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = sizeof( Skyboxlayout ) / sizeof( Skyboxlayout[0] );

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    g_pSkyboxTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V_RETURN( pd3dDevice->CreateInputLayout( Skyboxlayout, numElements, PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &g_pSkyboxVLayout ) );

	// Create and fill vertex buffer
	SkyboxVertex sky_vertices[Skyboxdetail * Skyboxdetail + Skyboxdetail]; 	
	// Create sphere vertices
	int counter = 0;
	float radius = size;
	// from phi = -pi to pi
	for(float phi = -D3DX_PI ; phi < D3DX_PI ; phi += (2*D3DX_PI/(float)Skyboxdetail) )
	{
		// from theta = 0 to 2pi
		for(float theta = 0 ; theta < 2*D3DX_PI; theta += (2*D3DX_PI/(float)Skyboxdetail) )
		{
			sky_vertices[counter].Pos = 
				D3DXVECTOR3(
					radius*cos(phi)*cos(theta),
					radius*sin(phi),
					radius*cos(phi)*sin(theta)				
				);
			counter++;
		}
	}

	D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SkyboxVertex ) * Skyboxdetail * Skyboxdetail;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = sky_vertices;
    hr = pd3dDevice->CreateBuffer( &bd, &InitData, &g_pSkyboxVertexBuffer );
    if( FAILED( hr ) )
        return hr;

	// Create index buffer
    DWORD sky_indices[Skyboxdetail * Skyboxdetail * 6];

	int i = 0;
	int curPoint, offset;
	for(int deltaV=0; deltaV<Skyboxdetail; deltaV++)
	{
		for(int deltaU=0; deltaU<Skyboxdetail; deltaU++)
		{
			curPoint = deltaV*Skyboxdetail + deltaU;

			// wrap edges
			if( (curPoint+1)%Skyboxdetail == 0 )
				offset = Skyboxdetail;
			else offset = 0;

			sky_indices[i++] = curPoint; // 1
			sky_indices[i++] = curPoint + 1 - offset; // 4
			sky_indices[i++] = curPoint + 1 + Skyboxdetail - offset; // 3

			sky_indices[i++] = curPoint; // 1
			sky_indices[i++] = curPoint + 1 + Skyboxdetail - offset; // 3
			sky_indices[i++] = curPoint + Skyboxdetail; // 2
		}
	}

    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( DWORD ) * Skyboxdetail * Skyboxdetail * 6;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData2;
    InitData2.pSysMem = sky_indices;
    hr = pd3dDevice->CreateBuffer( &bd, &InitData2, &g_pSkyboxIndexBuffer );
    if( FAILED( hr ) )
        return hr;

	return S_OK;
}

void Skybox::Render( D3DXMATRIX* pCamView, D3DXMATRIX* pCamProj )
{
	// Set buffers and topology
    UINT stride = sizeof( SkyboxVertex );
    UINT offset = 0;
	pd3dDevice->IASetInputLayout( g_pSkyboxVLayout );
    pd3dDevice->IASetVertexBuffers( 0, 1, &g_pSkyboxVertexBuffer, &stride, &offset );
	pd3dDevice->IASetIndexBuffer( g_pSkyboxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	//D3DXMATRIX mProj;
    D3DXMATRIX mFakeView;
    D3DXMATRIX mFakeViewProj;
	D3DXMATRIX mProj = *pCamProj;

	// Get the projection & view matrices from the camera class then fake em up
	mFakeView = *pCamView;
	mFakeView._41 = 0; // <_41, _42, _43> corresponds to distance vector, Skybox has "no distance"
	mFakeView._42 = 0;
	mFakeView._43 = 0;
	mProj = *pCamProj;
	mFakeViewProj = mFakeView * mProj;

	// Set effect variables
    g_pmViewProj->SetMatrix( ( float* )&mFakeViewProj );

	g_pSkyboxTechnique->GetPassByIndex( 0 )->Apply( 0 );
	pd3dDevice->DrawIndexed( (Skyboxdetail*Skyboxdetail)*6, 0, 0 );
}
