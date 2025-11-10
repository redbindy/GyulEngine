cbuffer CBFrame : register(b0)
{
    float3 cameraPos;
    float dummy;
    float4x4 viewProj;
};

cbuffer CBWorldMatrix : register(b1)
{
    float4x4 world;
    float4x4 invTrans;
};

cbuffer CBMaterial : register(b2)
{
    float3 ambientColor;
    float shininess;
    float3 diffuseColor;
    float dummy0;
    float3 specularColor;
    float dummy1;

    bool bUseTexture;
};

Texture2D sTexture : register(t0);
SamplerState sSampler : register(s0);

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};