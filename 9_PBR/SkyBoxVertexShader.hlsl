#include "SkyBoxShared.hlsli"

VS_SKYOUT main(VS_SKYIN input)
{
    VS_SKYOUT output = (VS_SKYOUT) 0;
    
    output.Dir = input.Pos;
    
    float3x3 viewRot = (float3x3)View;
    
    output.PosH = float4(mul((float3) input.Pos, viewRot), 1.0f); // S, R
    output.PosH = mul(output.PosH, Projection);
    
    output.PosH.z = output.PosH.w;
    
    return output;
}