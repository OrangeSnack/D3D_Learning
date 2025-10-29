#include "Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, skinWorldMat);
    output.Pos = mul(output.Pos, World);
    output.W_Pos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    matrix normMat = mul(NormalMatrix, skinNormMat);
    
    output.Norm = mul(input.Norm, (float3x3) normMat);
    output.Tan = mul(input.Tan, (float3x3) normMat);
    output.BiNorm = mul(input.BiNorm, (float3x3) normMat);
    output.Tex = input.Tex;
    
    return output;
}