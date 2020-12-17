//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 DiffuseMtrl;
	float4 DiffuseLight;
	float3 LightVecW;
    float gTime;
    float4 AmbientMtrl;
    float4 AmbientLight;
    float4 SpecularMtrl;
    float4 SpecularLight;
    float SpecularPower;
    float3 EyePosW;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 NormalW : NORMAL;
    float3 PosW : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 NormalW : NORMAL;
    float3 PosW : POSITION;
    float2 Tex : TEXCOORD;
};

//------------------------------------------------------------------------------------
// Vertex Shader - Implements Gouraud Shading
//------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0 )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = mul( Pos, World );

    // Convert from local space to world space
    output.PosW = (output.Pos).xyz;

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    
    // Convert from local space to world space 
    // W component of vector is 0 as vectors cannot be translated
    output.NormalW = normalize(mul(float4(NormalL, 0.0f), World).xyz);

    output.Tex = Tex;

    return output;
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
    float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);

    // Compute the vector from the vertex to the eye position.
    // input.Pos is currently the position in world space
    float3 toEye = normalize(EyePosW - input.PosW);

    // Compute the reflection vector.
    float3 r = reflect(-LightVecW, input.NormalW);

    // Determine how much (if any) specular light makes it into the eye.
    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);

    // Compute Colour
    float3 diffuse = max(dot(LightVecW, input.NormalW), 0.0f) * (DiffuseMtrl * DiffuseLight).rgb;
    float3 ambient = (AmbientMtrl * AmbientLight).rgb;
    float3 specular = specularAmount * ((SpecularMtrl * SpecularLight).rgb);

    float4 Color;
    clip(textureColour.a - 0.25f);
    Color.rgb = (diffuse + ambient + specular) * textureColour.rgb;
    Color.a = DiffuseMtrl.a;

    return Color;
}
