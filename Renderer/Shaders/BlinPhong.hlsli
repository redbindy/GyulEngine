#include "Common.hlsli"

struct PSBlinPhongInput
{
    float4 pos : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normalWorld : NORMAL;
    float2 uv : TEXCOORD;
};

typedef PSBlinPhongInput VSBlinPhongOutput;