#include "Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    Matrix skinNorm;
    
    if (input.BoneIdx.r != 65535)
    {
        float4 tempPos = { 0.0f, 0.0f, 0.0f, 0.0f };
        //tempPos += mul(mul(input.Pos, boneOffsetMat[input.BoneIdx.r]), boneMat[input.BoneIdx.r]);
        //tempPos += mul(input.Pos, boneMat[input.BoneIdx.g]) * input.BoneWeight.g;
        //tempPos += mul(input.Pos, boneMat[input.BoneIdx.b]) * input.BoneWeight.b;
        //tempPos += mul(input.Pos, boneMat[input.BoneIdx.a]) * input.BoneWeight.a;
        
        Matrix tempMat = mul(boneOffsetMat[input.BoneIdx.r], boneMat[input.BoneIdx.r]);
        tempPos += mul(input.Pos, tempMat);
        
        output.Pos = mul(tempPos, World);
   
        //output.Pos = mul(input.Pos, World);
    }
    else
    {
        output.Pos = mul(input.Pos, World);
    }
    output.W_Pos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    //float4x4 normMat = mul(NormalMatrix, skinNorm);
    matrix normMat = NormalMatrix;
    
    output.Norm = mul(input.Norm, (float3x3) normMat);
    output.Tan = mul(input.Tan, (float3x3) normMat);
    output.BiTan = mul(input.BiTan, (float3x3) normMat);
    output.Tex = input.Tex;
    
    return output;
}