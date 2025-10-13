struct VSBasicInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PSBasicInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

typedef PSBasicInput VSBasicOutput;

cbuffer CBFrame : register(b0)
{
    float3 cameraPos;
    float dummy;
    float4x4 viewProj;
};

cbuffer CBWorldMatrix : register(b1)
{
    float4x4 world;
};