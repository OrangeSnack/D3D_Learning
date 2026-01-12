#include "PBRShared.hlsli"

float main(PS_INPUT input) : SV_Depth
{
    float depth = input.Pos.z / input.Pos.w;
    
    return depth;
}