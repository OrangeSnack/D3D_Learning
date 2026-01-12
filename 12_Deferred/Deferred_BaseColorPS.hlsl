// 픽셀 셰이더(쉐이더/셰이더).
#include "PBRShared.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 texColor = _albedo.Sample(_sp0, input.Tex);
    clip(texColor.a - 0.5f);
    
    return texColor;
}