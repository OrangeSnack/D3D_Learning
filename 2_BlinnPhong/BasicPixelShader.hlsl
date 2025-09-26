// «»ºø ºŒ¿Ã¥ı(Ω¶¿Ã¥ı/ºŒ¿Ã¥ı).
#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    //phong ∏µ®
    float4 finalColor = 0;
    float4 texColor = _tex0.Sample(_sp0, input.Tex);
    float3 lightDir = normalize((float3) vLightDir);
    float3 camDir = normalize(input.CamDir);
    
    //reflect
    float4 cubemapColor = _cubemap.Sample(_sp0, reflect(camDir, input.Norm));
    
    //ambient
    float4 ambientColor = ambient * matAmbient * texColor * cubemapColor;
    
    //diffuse
    float4 diff = saturate(dot(input.Norm, -lightDir)) * diffuse * vLightColor;
    float4 diffuseColor = diff * matDiffuse * texColor;
    
    //specular
    float4 specularColor = pow(saturate(dot(reflect(lightDir, input.Norm), -camDir)), shiness) * specular * matSpecular * vLightColor;
    
    
    finalColor.rgb = (ambientColor.rgb + diffuseColor.rgb + specularColor.rgb);
    finalColor.a = 1;
   
    return finalColor;
}