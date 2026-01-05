// ÇÈ¼¿ ¼ÎÀÌ´õ(½¦ÀÌ´õ/¼ÎÀÌ´õ).
#include "PBRShared.hlsli"

// GGX
float GGX(float3 N, float3 H, float alpha)
{
    float NoH = saturate(dot(N, H));
    float a2 = alpha * alpha;
    float d = NoH * NoH * (a2 - 1.0f) + 1.0f;
    return a2 / (3.141592f * (d * d));
}

// Fresnel
float3 Schlick(float3 H, float3 V, float3 F0)
{
    float cosTheta = saturate(dot(H, V));
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

// Sub
float GSub(float NV, float NL, float alpha)
{
    float a2 = pow(alpha, 2);
    
    float gv = NV + sqrt(a2 + (1.0f - a2) * NV * NV);
    float gl = NL + sqrt(a2 + (1.0f - a2) * NL * NL);

    return (NV * NL) / (gv * gl);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // ¾ËÆÄÅ×½ºÆ®
    float4 texColor = _albedo.Sample(_sp0, input.Tex);
    clip(texColor.a - 0.5f);
    
    // ÅØ½ºÃ³ »ùÇÃ¸µ
    float3 albedo = _albedo.Sample(_sp0, input.Tex).rgb;
    float metalic = _metalic.Sample(_sp0, input.Tex).r;
    float roughness = _roughness.Sample(_sp0, input.Tex).r;
    float ao = _ambientOcclusion.Sample(_sp0, input.Tex).r;
    float3 emissive = _emissive.Sample(_sp0, input.Tex).rgb;
    
    // º¤ÅÍ
    float3 N = normalize(input.Norm);
    float3 V = normalize(mCamPos.xyz - input.W_Pos.xyz);
    float3 L = normalize(mLightDir.xyz);
    float3 H = normalize(V + L);
    
    // ±âº»¹Ý»çÀ² ±¸ÇÏ±â
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metalic);
    
    // BRDF ±¸¼º¿ä¼Ò
    float NV = saturate(dot(N, V));
    float NL = saturate(dot(N, L));
    
    float alpha = max(roughness * roughness, 0.001f);
    float D = GGX(N, H, alpha);
    float3 F = Schlick(H, V, F0);
    float G = GSub(NV, NL, alpha);
    
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalic);
    float3 diffuseBRDF = kd * albedo / 3.141592f;
    float3 specularBRDF = (D * G * F) / (4.0f * NV * NL);
    
    // ½¦µµ¿ì¸Ê Ã³¸®
    float currentShadowDepth = input.S_Pos.z / input.S_Pos.w; // ½¦µµ¿ì¸Ê ±âÁØ NDC ZÁÂÇ¥
    float2 shadowUV = input.S_Pos.xy / input.S_Pos.w;
    
    shadowUV.y *= -1.0f;
    shadowUV = (shadowUV * 0.5f) + 0.5f;
    
    float shadowFactor = 0.0f;
    
    if (shadowUV.x >= 0.0f && shadowUV.x <= 1.0f && shadowUV.y >= 0.0f && shadowUV.y <= 1.0f)
    {
        // Normal
        {
            float sampleShadowDepth = _shadowmap.Sample(_sp0, shadowUV).r;
        
            if (currentShadowDepth > sampleShadowDepth + 0.001f)
            {
                shadowFactor = 0.0f;
            }
            else
            {
                shadowFactor = 1.0f;
            }
        }
        
        //// 9 Texel
        //{
        //    float2 offset[9] =
        //    {
        //        float2(-1, -1), float2(0, -1), float2(1, -1),
        //        float2(-1, 0), float2(0, 0), float2(1, 0),
        //        float2(-1, 1), float2(0, 1), float2(1, 1)
        //    };
        //    float texelSize = 1.0f / 8192.0f;
        //    shadowFactor += _shadowmap.SampleCmpLevelZero(_spc0, shadowUV, currentShadowDepth - 0.001f);
        //}
        
        //// Æò±Õ°ª
        //shadowFactor /= 9.0f;
    }
    
    float3 light = mLightColor.rgb;
    float3 color = ((diffuseBRDF * shadowFactor) + specularBRDF) * light * NL;
    float4 finalColor = float4(color, 1.0f);
    
    return finalColor;
}