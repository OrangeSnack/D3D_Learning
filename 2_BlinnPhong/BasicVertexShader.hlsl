// ¡§¡° ºŒ¿Ã¥ı.
#include "Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.W_Pos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
    
    output.Norm = normalize(mul(input.Norm, (float3x3) World));
    
    float4 calPos = mul(input.Pos, World);
    output.CamPos = camPos;

    return output;
}