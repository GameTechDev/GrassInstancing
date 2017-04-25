/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DXUT.h"
#include "SDKmisc.h"


struct SkyboxVertex
{
    D3DXVECTOR3 Pos;
};

#define Skyboxdetail 5 // Skyboxdetail x Skyboxdetail vertices
class Skybox
{
public:
	Skybox( ID3D10Device* pd3dDevice, float size );
	~Skybox( void );

	HRESULT FillBuffers( void );
	HRESULT InitEffects( void );
	void Render(  D3DXMATRIX* pCamView, D3DXMATRIX* pCamProj );
	ID3D10ShaderResourceView*			g_pSRVSkyboxCubemap; // need to share with water class


private:
	ID3D10InputLayout*          		g_pSkyboxVLayout;
	ID3D10Buffer*						g_pSkyboxVertexBuffer;
	ID3D10Buffer*						g_pSkyboxIndexBuffer;	float size;
	ID3D10Effect*               		g_pSkyboxEffect;
	ID3D10EffectScalarVariable*			g_pfSunAlpha;
	ID3D10EffectScalarVariable*			g_pfSunTheta;
	ID3D10EffectScalarVariable*			g_pfSunShininess;
	ID3D10EffectScalarVariable*			g_pfSunStrength;
	ID3D10EffectMatrixVariable*			g_pmViewProj;
	ID3D10EffectShaderResourceVariable*	g_pSRVSkyboxCubemapVariable;
	ID3D10EffectTechnique*				g_pSkyboxTechnique;
	ID3D10Device*						pd3dDevice;
};