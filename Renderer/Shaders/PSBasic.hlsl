#include "Basic.hlsli"

float4 main(PSBasicInput input) : SV_TARGET
{
    const float BRIGHTNESS = 0.5f;
    
    const float4 color = bUseTexture ? sTexture.Sample(sSamplerState, input.uv) : float4(BRIGHTNESS, BRIGHTNESS, BRIGHTNESS, 1.f);
	
    return color;
}