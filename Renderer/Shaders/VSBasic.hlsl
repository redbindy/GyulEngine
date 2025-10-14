#include "Basic.hlsli"

VSBasicOutput main(VSBasicInput input)
{
    VSBasicOutput output;
    output.normal = input.normal;
    output.uv = input.uv;
    
    float4 pos = float4(input.pos, 1.f);
    
    const float4x4 wvpMat = mul(world, viewProj);
    
    output.pos = mul(pos, wvpMat);
	
    return output;
}