#include "Common.hlsli"

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 1.f);
	
    return output;
}