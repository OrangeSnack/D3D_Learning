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
    if (normalMap.r < 0.01f && normalMap.g < 0.01f && normalMap.b < 0.01f)
    {
        normal = normalize(input.Norm);
    }
    else
    {
        normalMap = normalMap * 2.0f - 1.0f;
        float3x3 tbn = float3x3(normalize(input.Tan), normalize(input.BiTan), normalize(input.Norm));
        normal = normalize(mul(normalMap, tbn));
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
    
    finalColor.rgb = (ambientColor.rgb + diffuseColor.rgb) * texColor.rgb + specularColor.rgb + emitColor.rgb;
    finalColor.a = 1;
   
    return finalColor;
}