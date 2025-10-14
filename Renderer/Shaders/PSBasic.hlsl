#include "Basic.hlsli"

Texture2D sTexture : register(t0);
SamplerState sSamplerState : register(s0);

float4 main(PSBasicInput input) : SV_TARGET
{
    const float4 color = sTexture.Sample(sSamplerState, input.uv);
	
    return color;
}