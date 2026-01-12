#include "PBRShared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float ao = _ambientOcclusion.Sample(_sp0, input.Tex).r;
    float roughness = _roughness.Sample(_sp0, input.Tex).r;
    float metalic = _metalic.Sample(_sp0, input.Tex).r;
    
    return float4(ao, roughness, metalic, 1.0f);
}