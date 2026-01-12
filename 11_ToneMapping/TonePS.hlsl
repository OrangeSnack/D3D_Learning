Texture2D _tex : register(t0);
SamplerState _sp0 : register(s0);

cbuffer ToneBuffer : register(b0)
{
    float m_Exposure;
    float m_Brightness;
    float2 m_TonePadding;
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 main(PS_INPUT _input) : SV_TARGET
{
    float3 color = _tex.Sample(_sp0, _input.uv).rgb;
    color *= m_Brightness;
    
    float exposureFactor = pow(2.0f, m_Exposure);
    color *= exposureFactor;
    
    float3 toneColor = ACESFilm(color);
    
    float3 gammaColor = pow(toneColor, 1.0 / 2.2);
    float4 finalColor = float4(gammaColor, 1.0f);
    
    return finalColor;
}