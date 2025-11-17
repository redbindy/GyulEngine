#include "Common.hlsli"

struct VSBlinnPhongOutput
{
    float4 pos : SV_Position;
    float3 posWorld : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

typedef VSBlinnPhongOutput PSBlinnPhongInput;