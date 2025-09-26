// ¡§¡° ºŒ¿Ã¥ı.
#include "Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
    
    output.Norm = normalize(mul(input.Norm, NormalMatrix));
    
    float4 calPos = mul(input.Pos, World);
    output.CamDir = normalize(calPos - camPos);
    
    return output;
}