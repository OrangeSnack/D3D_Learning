TextureCube _cubemap : register(t0);
SamplerState _sample : register(s0);

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

struct VS_SKYOUT
{ 
    float4 PosH : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

struct VS_SKYIN
{
    float3 Pos : POSITION;
};
