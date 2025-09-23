// «»ºø ºŒ¿Ã¥ı(Ω¶¿Ã¥ı/ºŒ¿Ã¥ı).
#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = 0;
    
    float intensity = saturate(dot(normalize((float3) vLightDir), input.Norm));
    float shadow = lerp(0.2f, 1.5f, intensity);
    
    float4 texColor = _tex0.Sample(_sp0, input.Tex);
    float3 cubemapColor = _cubemap.Sample(_sp0, reflect(input.Dir, input.Norm));
    
    finalColor.rgb = texColor.rgb * vLightColor.rgb * cubemapColor * shadow;
    finalColor.a = 1;
   
    return finalColor;
    
    // ∂Û¿Ã∆√X
    //return _tex0.Sample(_sp0, input.Tex);
    
    // ≈•∫Í∏ ∏∏
    //finalColor.rgb = cubemapColor;
    //finalColor.a = 1;
}