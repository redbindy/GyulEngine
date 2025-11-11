#include "Basic.hlsli"

VSBasicOutput main(const VSInput input)
{
    const float4 worldPos = mul(float4(input.pos, 1.0f), world);
    const float4 worldNormal = mul(float4(input.normal, 0.0f), invTrans);
    
    VSBasicOutput output;
    output.pos = mul(worldPos, viewProj);
    output.normal = normalize(worldNormal.xyz);
    output.uv = input.uv;
    
    return output;
}