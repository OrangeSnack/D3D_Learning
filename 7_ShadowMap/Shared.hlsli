Texture2D _tex0 : register(t0);
Texture2D _norm : register(t1);
Texture2D _spec : register(t2);
Texture2D _emis : register(t3);
TextureCube _cubemap : register(t4);
Texture2D _shadowmap : register(t5);

SamplerState _sp0 : register(s0);

cbuffer TransBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix NormalMatrix;
}

cbuffer MatBuffer : register(b1)
{
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

cbuffer ShadowVP : register(b2)
{
    matrix ShadowView;
    matrix ShadowProjection;
}

cbuffer BoneWorldBuffer : register(b3)
{
    matrix boneMat[256];
}

cbuffer BoneOffSetBuffer : register(b4)
{
    matrix boneOffsetMat[256];
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 W_Pos : POSITION;    // 월드 포지션
    float3 Norm : NORMAL;
    float3 Tan : TANGENT;
    float3 BiTan : BITANGENT;
    float2 Tex : TEXCOORD0;     // 텍스쳐 UV
    float4 S_Pos : TEXCOORD1;   // 쉐도우 포지션
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float3 Tan : TANGENT;
    float3 BiTan : BITANGENT;
    float2 Tex : TEXCOORD0;
    int4 BoneIdx : BONEINDEX;
    float4 BoneWeight : BONEWEIGHT;
};
