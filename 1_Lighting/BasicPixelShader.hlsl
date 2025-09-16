// «»ºø ºŒ¿Ã¥ı(Ω¶¿Ã¥ı/ºŒ¿Ã¥ı).
#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = 0;
    
    //do NdotL lighting for 2 lights
    float intensity = saturate(dot(normalize((float3) vLightDir), input.Norm));
    finalColor += intensity * vLightColor;
    finalColor.a = 1;
    return finalColor;
}