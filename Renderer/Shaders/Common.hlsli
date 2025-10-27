struct VSBasicInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer CBFrame : register(b0)
{
    float3 cameraPos;
    float4x4 viewProj;
};

cbuffer CBWorldMatrix : register(b1)
{
    float4x4 world;
    float4x4 invTransNormal;
};

cbuffer CBMaterial : register(b3)
{
    bool bUseTexture;
}

Texture2D sTexture : register(t0);
SamplerState sSamplerState : register(s0);