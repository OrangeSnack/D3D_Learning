// ÇÈ¼¿ ¼ÎÀÌ´õ(½¦ÀÌ´õ/¼ÎÀÌ´õ).
#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    // ¾ËÆÄÅ×½ºÆ®
    float4 texColor = _tex0.Sample(_sp0, input.Tex);
    clip(texColor.a - 0.1f);
    
    //Blinnphong ¸ðµ¨
    float4 finalColor = 0;
    float4 emitColor = _emis.Sample(_sp0, input.Tex);
    float3 lightDir = normalize((float3) vLightDir);
    float3 camDir = normalize(input.W_Pos - camPos);
    
    // normalMap
    float3 normalMap = _norm.Sample(_sp0, input.Tex).xyz;
    float3 normal;
    if (all(normalMap < 0.01f))
    {
        normal = normalize(input.Norm);
    }
    else
    {
        normalMap = normalMap * 2.0f - 1.0f;
        float3x3 tbn = float3x3(normalize(input.Tan), normalize(input.BiTan), normalize(input.Norm));
        normal = normalize(mul(tbn, normalMap));
    }
    
    //ambient
    float4 ambientColor = ambient * matDiffuse;
    
    //diffuse
    float4 intensity = saturate(dot(normal, -lightDir));
    float4 diff = intensity * diffuse * vLightColor;
    float4 diffuseColor = diff * matDiffuse;
    
    //specular
    float3 middleVector = normalize(-lightDir + -camDir);
    float4 specularImage = _spec.Sample(_sp0, input.Tex);
    float4 specularColor = pow(saturate(dot(middleVector, normal)), shiness) * intensity * specularImage * specular * matSpecular * vLightColor;
   
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
        //    float sum = 0.0f;
            
        //    // 9¹ø »ùÇÃ¸µ
        //    for (int i = 0; i < 9; i++)
        //    {
        //        float2 uv = shadowUV + offset[i] * texelSize;
        //        sum += _shadowmap.SampleCmpLevelZero(_spc1, uv, currentShadowDepth - 0.001f);
        //    }
            
        //    // Æò±Õ°ª
        //    shadowFactor = sum / 9.0f;
        //}
    }
    
    finalColor.rgb = (ambientColor.rgb + (diffuseColor.rgb * shadowFactor)) * texColor.rgb + specularColor.rgb + emitColor.rgb;
    finalColor.a = 1;
    
    return finalColor;
}