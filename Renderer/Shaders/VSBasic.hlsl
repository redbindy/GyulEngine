#include "Basic.hlsli"

VSBasicOutput main(VSBasicInput input)
{
    VSBasicOutput output;
    
    const float4x4 wvpMat = mul(world, viewProj);
    
    const float4 pos = float4(input.pos, 1.f);
    output.pos = mul(pos, wvpMat);
    
    float4 normal = float4(input.normal, 1.f);
    normal = mul(normal, invTransNormal);
    
    output.normal = normal.xyz;
    
    output.uv = input.uv;
	
    return output;
}