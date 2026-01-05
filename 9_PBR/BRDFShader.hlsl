// ÇÈ¼¿ ¼ÎÀÌ´õ(½¦ÀÌ´õ/¼ÎÀÌ´õ).
#include "PBRShared.hlsli"

// GGX
float ndfGGX(float3 N, float3 H, float alpha)
{
    float NH = saturate(dot(N, H));
    float a2 = alpha * alpha;
    float d = (NH * NH) * (a2 - 1.0f) + 1.0f;
    return a2 / (3.141592f * (d * d));
}

// Fresnel
float3 Fresnel(float3 H, float3 V, float3 F0)
{
    float HV = saturate(dot(H, V));
    return F0 + (float3(1.0f, 1.0f, 1.0f) - F0) * pow(1.0f - HV, 5.0f);
}

// GeoSchlick
float GeoSchlick(float NdotX, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) * 0.125f;
    return NdotX / (NdotX * (1.0f - k) + k);

}

// GeoSmith
float geoSmith(float NV, float NL, float roughness)
{
    
    float gv = GeoSchlick(NV, roughness);
    float gl = GeoSchlick(NL, roughness);

    return gv * gl;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // ¾ËÆÄÅ×½ºÆ®
    float4 texColor = _albedo.Sample(_sp0, input.Tex);
    clip(texColor.a - 0.5f);
    
    // ÅØ½ºÃ³ »ùÇÃ¸µ
    float3 albedo = _albedo.Sample(_sp0, input.Tex).rgb;
    float metalic = _metalic.Sample(_sp0, input.Tex).r;// * mMetalic;
    float roughness = _roughness.Sample(_sp0, input.Tex).r;// * mRoughness;
    float ao = _ambientOcclusion.Sample(_sp0, input.Tex).r;// * mAoStrength;
    float3 emissive = _emissive.Sample(_sp0, input.Tex).rgb;// * mEmissive;
    
    // ³ë¸Ö
    float3 normalMap = _normal.Sample(_sp0, input.Tex).xyz;
    normalMap = normalMap * 2.0f - 1.0f;
    float3x3 tbn = float3x3(normalize(input.Tan), normalize(input.BiTan), normalize(input.Norm));

    // º¤ÅÍ
    float3 N = normalize(mul(tbn, normalMap));
    float3 V = normalize(mCamPos.xyz - input.W_Pos.xyz);
    float3 L = normalize(-mLightDir.xyz);
    float3 H = normalize(V + L);
    
    // ±âº»¹Ý»çÀ² ±¸ÇÏ±â
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metalic);
    
    // BRDF ±¸¼º¿ä¼Ò
    float NV = saturate(dot(N, V));
    float NL = saturate(dot(N, L));
    
    float alpha = max(roughness * roughness, 0.001f);
    float D = ndfGGX(N, H, alpha);
    float3 F = Fresnel(H, V, F0);
    float G = geoSmith(NV, NL, roughness);
    
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalic);
    float3 diffuseBRDF = kd * albedo / 3.141592f;
    float3 specularBRDF = (D * F * G) / max(4.0f * NL * NV, 0.001f);
    
    // ½¦µµ¿ì¸Ê Ã³¸®
    float currentShadowDepth = input.S_Pos.z / input.S_Pos.w;   // ½¦µµ¿ì¸Ê ±âÁØ NDC ZÁÂÇ¥
    float2 shadowUV = input.S_Pos.xy / input.S_Pos.w;
    
    shadowUV.y *= -1.0f;
    shadowUV = (shadowUV * 0.5f) + 0.5f;
    
    float shadowFactor = 1.0f;
    
    if (shadowUV.x >= 0.0f && shadowUV.x <= 1.0f && shadowUV.y >= 0.0f && shadowUV.y <= 1.0f)
    {
        // Normal
        {
            float sampleShadowDepth = _shadowmap.Sample(_sp0, shadowUV).r;
        
            if (currentShadowDepth > sampleShadowDepth + 0.001f)
            {
                shadowFactor = 0.0f;
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
    float3 direct = (diffuseBRDF + specularBRDF) * light * NL;
    float3 color = direct * shadowFactor + emissive;
    float4 finalColor = float4(pow(color, 1.0f / 2.2f), 1.0f);
    
    return finalColor;
    //return float4(albedo * shadowFactor, 1.0f);
}