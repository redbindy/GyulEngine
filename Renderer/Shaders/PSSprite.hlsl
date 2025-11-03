#include "Sprite.hlsli"

float4 main(PSSpriteInput input) : SV_TARGET
{
    const float BRIGHTNESS = 0.8f;
    
    return bUseTexture ? sTexture.Sample(sSamplerState, input.uv) : float4(BRIGHTNESS, BRIGHTNESS, BRIGHTNESS, 1.f);
}