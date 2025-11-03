#include "Sprite.hlsli"

VSSpriteOutput main(VSBasicInput input)
{
    VSSpriteOutput output;
    
    const float4x4 wvpMat = mul(world, viewProj);
    
    const float4 pos = float4(input.pos, 1.f);
    output.pos = mul(pos, wvpMat);
    
    output.uv = input.uv;
    
    return output;
}