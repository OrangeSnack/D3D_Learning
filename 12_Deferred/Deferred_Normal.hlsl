#include "PBRShared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 normalMap = _normal.Sample(_sp0, input.Tex).xyz;
    normalMap = normalize(normalMap * 2.0f - 1.0f);
    float3x3 tbn = float3x3(normalize(input.Tan), normalize(input.BiTan), normalize(input.Norm));
    
    float3 worldNormal = normalize(mul(normalMap, tbn));
    return float4(worldNormal * 0.5f + 0.5f, 1.0f);
}