#include "Common.hlsli"

struct VSBasicOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

typedef VSBasicOutput PSBasicInput;