#include "Basic.hlsli"

float4 main(PSBasicInput input) : SV_TARGET
{    
    return bUseTexture ? sTexture.Sample(sSampler, input.uv) : float4(1.0f, 1.0f, 1.0f, 1.0f);
}