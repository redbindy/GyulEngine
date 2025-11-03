#include "Common.hlsli"

struct PSSpriteInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

typedef PSSpriteInput VSSpriteOutput;