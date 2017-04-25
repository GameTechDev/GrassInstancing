/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------
// File: BasicTNL.fx
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbNeverChanges
{
	float4	g_vWaveDistortX = float4( 3.0f, 0.4f, 0.0f, 0.3f );
	float4	g_vWaveDistortZ = float4( 3.0f, 0.4f, 0.0f, 0.3f );
	float4	g_vWaveDistortY = float4( -1.0f, -0.133f, -0.333f, -0.10f );
	float4	g_vWaveDirX = float4( -0.006f, -0.012f, 0.024f, 0.048f );
	float4	g_vWaveDirZ = float4( -0.003f, -0.006f, -0.012f, -0.048f );
	float4	g_vWaveSpeed = float4( 0.3f, 0.7f, 0.6f, 1.4f );
	float	g_fPIx2 = 6.28318530f;
	float4	g_vLightingWaveScale = float4( 0.35f, 0.10f, 0.10f, 0.03f );
	float4	g_vLightingScaleBias = float4( 0.6f, 0.7f, 0.2f, 0.0f );
};

cbuffer cbChangeOnResize
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame
{
	matrix Orientation;
    matrix World;
    matrix View;
    float g_fTime;
};


BlendState AlphaBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState QuadAlphaBlendState
{
    AlphaToCoverageEnable = TRUE;
	BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState EnableMSAA
{
    CullMode = BACK;
    MultisampleEnable = TRUE;
};
    
BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

DepthStencilState EnableDepthWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

DepthStencilState DisableDepthWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

DepthStencilState DisableDepthTest
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = ALWAYS;
};


struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

struct VS_INPUT_TREE
{
    float3 Pos			: POSITION;         //position
    float2 Tex			: TEXCOORD0;        //texture coordinate
    float3 offPos		: offPos;          // offsetPosition
    float2 scale		: scale;
};

struct VS_INPUT_GRASS
{
    float3 Pos			: POSITION;         //position
    float2 Tex			: TEXCOORD0;        //texture coordinate
    float2 vPPos		: vPPos;            // patchPosition
};


struct PS_INPUT_GRASS
{
    float4 Pos			: SV_POSITION;
    float4 Color		: COLOR0;
    float2 Tex			: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos		= mul( input.Pos, World );
    output.Pos		= mul( output.Pos, View );
    output.Pos		= mul( output.Pos, Projection );
    output.Tex		= input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex );
}

PS_INPUT RenderTreeVS( VS_INPUT_TREE input )
{
    PS_INPUT output = (PS_INPUT)0;

    output.Pos		= mul( float4(input.Pos, 1), Orientation );
    output.Pos.x   *= input.scale.x;
    output.Pos.y   *= input.scale.y;
    output.Pos     += float4(input.offPos,1);

    output.Pos		= mul( output.Pos,  World );
    output.Pos		= mul( output.Pos, View );
    output.Pos		= mul( output.Pos, Projection );
    output.Tex		= input.Tex;


    return output;	
}

PS_INPUT_GRASS RenderGrassVS( VS_INPUT_GRASS v )
{
	PS_INPUT_GRASS o;
	//sinusoidal vertex motion for waving grass
	//pos + sumOverI(wavedirI * texcoordy * sin( xdirI * (xpos+time)) + ydirI * (ypos+time)))


	// use vertex pos x and y as inputs to sinusoidal warp 
	float4 vWaveVec = (g_vWaveDirX * v.Pos.x) + (g_vWaveDirZ * v.Pos.z);

	// add scaled time to move bumps according to speed
	vWaveVec += g_fTime * g_vWaveSpeed;

	// take frac of all 4 components
	vWaveVec = frac( vWaveVec );

	vWaveVec -= 0.8f;

	// *=2pi coords range from (-pi to pi)
	vWaveVec *= g_fPIx2;	// pi * 2.0
	
	// taylor series expansion replaced by actual sin fun
	vWaveVec = sin( vWaveVec );

	float4 vWaveDistortion;
	vWaveDistortion.x  = dot( vWaveVec, g_vWaveDistortX );
	vWaveDistortion.y  = dot( vWaveVec, g_vWaveDistortY );
	vWaveDistortion.zw = dot( vWaveVec, g_vWaveDistortZ );

	// attenuate sinusoidal warping by (1-tex0.y)^2
	float fSinWarp	 = 1.0f - v.Tex.y;
	fSinWarp		*= fSinWarp;
	vWaveDistortion *= fSinWarp;

	// Out position -- add sinusoidal warping to grass position
	float4 vGrassPos;
	vGrassPos.xyz = vWaveDistortion + v.Pos;
	vGrassPos.w   = 1.0; //v.Pos;
	vGrassPos.x  += v.vPPos.x;
	vGrassPos.z  += v.vPPos.y;
		
	o.Pos = mul( vGrassPos, World );
	o.Pos = mul( o.Pos, View );
	o.Pos = mul( o.Pos, Projection );

	// scale and add sin waves together
	// scale and bias color values 
	// (green is scaled more than red and blue)

	float fScaled = dot( vWaveVec, g_vLightingWaveScale );
	o.Color = (g_vLightingScaleBias.zzzw * -fScaled) + g_vLightingScaleBias.y;
	// Pass the tex coord through	
	o.Tex = v.Tex;
	
	return o;
}

float4 RenderGrassPS( PS_INPUT_GRASS input) : SV_Target
{
    //calculate lighting assuming light color is <1,1,1,1>
    //float fLighting  = saturate( dot( input.Norm, g_LightDir ) );
    float4 outputColor = txDiffuse.Sample( samLinear, input.Tex );
    outputColor.xyz   *= input.Color.xyz;
    return outputColor;
}

technique10 RenderGrass
{
    pass P0
    {          
        SetVertexShader( CompileShader( vs_4_0,RenderGrassVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderGrassPS() ) );
        
        SetBlendState( QuadAlphaBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepthWrite, 0 );
        SetRasterizerState( EnableMSAA );

    }
}

//--------------------------------------------------------------------------------------
technique10 RenderTree
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, RenderTreeVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );

    }

}

//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );

//        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
//        SetDepthStencilState( EnableDepthWrite, 0 );
    }

}

