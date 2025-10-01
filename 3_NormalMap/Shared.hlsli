Texture2D _tex0 : register(t0);
Texture2D _norm : register(t1);
Texture2D _spec : register(t2);
TextureCube _cubemap : register(t3);
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
    float4 W_Pos : POSITION;
    float3 Norm : NORMAL;
    float3 Tan : TANGENT;
    float3 BiNorm : BINORMAL;
    float2 Tex : TEXCOORD0;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
    float3 Norm : NORMAL;
    float3 Tan : TANGENT;
    float3 BiNorm : BINORMAL;
};
