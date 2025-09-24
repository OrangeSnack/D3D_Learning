Texture2D _tex0 : register(t0);
TextureCube _cubemap : register(t1);
SamplerState _sp0 : register(s0);

cbuffer _cb0 : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix NormalMatrix;
    
    float4 vLightDir;
    float4 vLightColor;
    float4 camPos;
    
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
    
    int shiness;
    float3 padding;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 CamDir : DIRECTION;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};
