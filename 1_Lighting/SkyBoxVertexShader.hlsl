#include "SkyBoxShared.hlsli"

VS_SKYOUT main(VS_SKYIN input)
{
    VS_SKYOUT output = (VS_SKYOUT) 0;
    
    output.TexCoord = input.Pos;
    
    float3x3 viewRot = (float3x3)View;
    float3 dir = mul(input.Pos.xyz, viewRot);
    float4 posH = mul(float4(dir, 1.0f), Projection);
    
    output.PosH = posH;
    output.PosH.z = output.PosH.w;
    
    return output;
}