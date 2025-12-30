#include "Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4x4 skinMat = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
    };
    
#ifdef VERTEX_SKINNING
    Matrix tempMat[4] =
    {
        mul(boneOffsetMat[input.BoneIdx.x], boneMat[input.BoneIdx.x]),
        mul(boneOffsetMat[input.BoneIdx.y], boneMat[input.BoneIdx.y]),
        mul(boneOffsetMat[input.BoneIdx.z], boneMat[input.BoneIdx.z]),
        mul(boneOffsetMat[input.BoneIdx.w], boneMat[input.BoneIdx.w])
    };
    
    skinMat = mul(input.BoneWeight.x, tempMat[0]);
    skinMat += mul(input.BoneWeight.y, tempMat[1]);
    skinMat += mul(input.BoneWeight.z, tempMat[2]);
    skinMat += mul(input.BoneWeight.w, tempMat[3]);
    
    skinMat = mul(skinMat, World);
#else
    skinMat = mul(skinMat, World);
#endif
    
    output.Pos = mul(input.Pos, skinMat);
    output.Pos = mul(output.Pos, ShadowView);
    output.Pos = mul(output.Pos, ShadowProjection);
    
    return output;
}