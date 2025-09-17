Texture2D _tex0 : register(t0);
SamplerState _sp0 : register(s0);

cbuffer _cb0 : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix NormalMatrix;
    float4 vLightDir;
    float4 vLightColor;
    float4 vOutputColor;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};


