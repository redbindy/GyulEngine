#include "Sprite.hlsli"

VSOutput main(VSInput input)
{
    const float4 worldPos = mul(float4(input.pos, 1.0f), world);
    
    VSOutput output;
    output.pos = mul(worldPos, viewProj);
    output.uv = input.uv;
    
    return output;
}