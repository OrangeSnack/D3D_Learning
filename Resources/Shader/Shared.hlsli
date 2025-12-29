Texture2D _tex0 : register(t0);
Texture2D _norm : register(t1);
Texture2D _spec : register(t2);
Texture2D _emis : register(t3);
TextureCube _cubemap : register(t4);
Texture2D _shadowmap : register(t5);

SamplerState _sp0 : register(s0);

cbuffer Cambuffer : register(b0)
{
    matrix View;
    matrix Projection;
    float4 camPos;
}

cbuffer Transbuffer : register(b1)
{
    matrix World;
    matrix NormalMatrix;
}

cbuffer LightBuffer : register(b2)
{
    float4 vLightDir;
    float4 vLightColor;
}

cbuffer MatBuffer : register(b3)
{
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
    
    int shiness;
    float3 padding;
}

cbuffer ShadowVP : register(b4)
{
    matrix ShadowView;
    matrix ShadowProjection;
}

cbuffer BoneWorldBuffer : register(b5)
{
    matrix boneMat[256];
}

cbuffer BoneOffSetBuffer : register(b6)
{
    matrix boneOffsetMat[256];
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 W_Pos : POSITION;    // ���� ������
    float3 Norm : NORMAL;
    float3 Tan : TANGENT;
    float3 BiTan : BITANGENT;
    float2 Tex : TEXCOORD0;     // �ؽ��� UV
    float4 S_Pos : TEXCOORD1;   // ������ ������
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
