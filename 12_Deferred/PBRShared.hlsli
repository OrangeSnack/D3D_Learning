Texture2D _albedo : register(t0);
Texture2D _normal: register(t1);
Texture2D _metalic : register(t2);
Texture2D _roughness : register(t3);
Texture2D _ambientOcclusion : register(t4);
Texture2D _emissive : register(t5);
Texture2D _shadowmap : register(t6);
TextureCube _specular : register(t7);
TextureCube _irradiance : register(t8);
Texture2D _brdflut : register(t9);

Texture2D _defAlbedo : register(t10);
Texture2D _defNormal : register(t11);
Texture2D _defARM : register(t12);
Texture2D _defPos : register(t13);

SamplerState _sp0 : register(s0);

cbuffer TransBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    matrix mNormalMatrix;
    float4 mCamPos;
}

cbuffer LightBuffer : register(b1)
{
    float4 mLightDir;
    float4 mLightColor;
}

cbuffer MatBuffer : register(b2)
{
    float4 mBaseColor;
    
    float mMetalic;
    float mRoughness;
    float mAoStrength;
    float mEmissive;
    
    int mUseOverride;
    float3 mOverridePadding;
}

cbuffer ShadowVP : register(b3)
{
    matrix ShadowView;
    matrix ShadowProjection;
}

cbuffer BoneWorldBuffer : register(b4)
{
    matrix boneMat[256];
}

cbuffer BoneOffSetBuffer : register(b5)
{
    matrix boneOffsetMat[256];
}

cbuffer ToneBuffer : register(b6)
{
    float mExposure;
    float mBrightness;
    float2 mTonePadding;
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
