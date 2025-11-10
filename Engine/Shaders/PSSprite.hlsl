#include "Sprite.hlsli"

float4 main(const PSInput input) : SV_TARGET
{
    return sTexture.Sample(sSampler, input.uv);
}