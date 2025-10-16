// «»ºø ºŒ¿Ã¥ı(Ω¶¿Ã¥ı/ºŒ¿Ã¥ı).
#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    //phong ∏µ®
    float4 finalColor = 0;
    float4 texColor = _tex0.Sample(_sp0, input.Tex);
    float3 lightDir = normalize((float3) vLightDir);
    float3 camDir = normalize(input.W_Pos - camPos);

    // normalMap
    float3 normalMap = _norm.Sample(_sp0, input.Tex) * 2.0f - 1.0f;
    float3x3 tbn =
    {
        input.Tan.x, input.Tan.y, input.Tan.z,
        input.BiNorm.x, input.BiNorm.y, input.BiNorm.z,
        input.Norm.x, input.Norm.y, input.Norm.z
    };
    
    float3 normal = normalize(mul(normalMap, tbn));
    
    //reflect
    float4 cubemapColor = _cubemap.Sample(_sp0, reflect(camDir, normal));
    
    //ambient
    float4 ambientColor = ambient * matAmbient * texColor;// * cubemapColor;
    //float4 ambientColor = cubemapColor * ambient;
    
    //diffuse
    float4 diff = saturate(dot(normal, -lightDir)) * diffuse * vLightColor;
    float4 diffuseColor = diff * matDiffuse * texColor;
    
    //specular
    float4 specularImage = _spec.Sample(_sp0, input.Tex);
    float4 specularColor = pow(saturate(dot(reflect(lightDir, normal), -camDir)), shiness) * specularImage * specular * matSpecular * vLightColor;
    
    
    finalColor.rgb = (ambientColor.rgb + diffuseColor.rgb + specularColor.rgb);
    finalColor.a = 1;
   
    return finalColor;
}