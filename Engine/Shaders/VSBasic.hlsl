#include "Basic.hlsli"

VSBasicOutput main(const VSInput input)
{
    const float4 worldPos = mul(float4(input.pos, 1.0f), world);
    
    VSBasicOutput output;
    output.pos = mul(worldPos, viewProj);
    
    return output;
}