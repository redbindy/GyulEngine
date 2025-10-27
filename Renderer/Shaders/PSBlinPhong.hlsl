#include "BlinPhong.hlsli"

cbuffer CBDirectoinalLight : register(b2)
{
    float3 dir;
    float3 strength;
    bool bOn;
}

cbuffer CBClassicLightingMaterial : register(b4)
{
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float shininess;
};

float4 main(PSBlinPhongInput input) : SV_TARGET
{
    const float BRIGHTNESS = 0.5f;
    
    float4 color = bUseTexture ? sTexture.Sample(sSamplerState, input.uv) : float4(BRIGHTNESS, BRIGHTNESS, BRIGHTNESS, 1.f);
    
    const float3 lightVec = -dir;
        
    const float3 normal = normalize(input.normalWorld);
        
    const float nDotL = max(dot(normal, lightVec), 0.f);
        
    const float3 lightStrength = strength * nDotL;
     
    const float3 toEye = input.posWorld - cameraPos;
        
    const float3 halfway = normalize(lightVec + toEye);
        
    const float hDotN = max(dot(halfway, normal), 0.f);
        
    const float3 result = (ambient + diffuse + specular * pow(hDotN, shininess)) * lightStrength;

    color += float4(result, 0.f);
    
    return color;
}