#include "BlinPhong.hlsli"

VSBlinPhongOutput main(VSBasicInput input)
{
    VSBlinPhongOutput output;
    
    const float4 pos = float4(input.pos, 1.f);
    
    output.pos = mul(pos, world);
    output.posWorld = output.pos.xyz;
    output.pos = mul(output.pos, viewProj);
    
    float4 normal = float4(input.normal, 0.f);
    normal = mul(normal, invTransNormal);
    
    output.normalWorld = normal.xyz;
    
    output.uv = input.uv;
	
    return output;
}