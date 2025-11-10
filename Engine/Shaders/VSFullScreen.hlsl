#include "Sprite.hlsli"

VSOutput main(const VSInput input)
{
	VSOutput output;
	output.pos = float4(input.pos, 1.0f);
	output.uv = input.uv;
	
	return output;
}