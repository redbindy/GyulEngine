#include "Common.hlsli"

struct PSBasicInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

typedef PSBasicInput VSBasicOutput;