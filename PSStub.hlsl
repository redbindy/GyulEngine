#include "Basic.hlsli"

float4 main(PSBasicInput input) : SV_Target
{
    const float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    return color;
}