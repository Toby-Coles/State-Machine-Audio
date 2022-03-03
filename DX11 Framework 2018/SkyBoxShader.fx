//--------------------------------------------------------------------------------------
// File: SkyBoxShader.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
// Texturing
//-------------------------------------------------------------------------------------

Texture2D skyboxTexture : register(t0);

SamplerState samLinear : register(s0);




//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm: NORMAL;
    float3 PosW : POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

//VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0)
//{
//
//    VS_OUTPUT output = (VS_OUTPUT)0;
//    output.Norm = normalize(NormalL);
//
//    output.Pos = mul(Pos, World);
//    output.PosW = normalize(EyePosW - output.Pos.xyz);
//
//    output.Pos = mul(output.Pos, View);
//    output.Pos = mul(output.Pos, Projection);
//    output.Tex = Tex;
//
//    return output;
//
//}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    // Convert from local space to world space 
   // W component of vector is 0 as vectors cannot be translated
    //float4 textureColour = txDiffuse(Sample(samLinear, input.Tex))
    return skyboxTexture.Sample(samLinear, 1.0 - input.Tex);
}
