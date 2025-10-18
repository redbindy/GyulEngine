#include "Basic.hlsli"

float4 main(VSBasicInput input) : SV_POSITION
{
    float4 outputPos = float4(input.pos, 1.f);
    
    const float4x4 wvpMat = mul(world, viewProj);
    
    outputPos = mul(outputPos, wvpMat);
    
    return outputPos;
}