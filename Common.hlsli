struct VSInput
{
    float3 pos : POSITION;
};

struct PSInput
{
    float4 pos : SV_Position;
};

typedef PSInput VSOutput;