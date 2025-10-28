#include "BlinPhong.hlsli"

#define MAX_LIGHTS (8)

cbuffer CBDirectionalLight : register(b2)
{
    float3 dir;
    float3 strength;
}

cbuffer CBClassicLightingMaterial : register(b4)
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float3 specular;
};

struct Light
{
    float3 position;
    float fallOffStart;
    float3 strength;
    float fallOffEnd;
    float3 direction;
    float spotPower;
};

cbuffer CBLight : register(b5)
{
    Light lights[MAX_LIGHTS];
};

float3 ComputeBlinnPhong(
    const float3 lightVec,
    const float3 lightStrength,
    const float3 pos,
    const float3 normal,
    const float3 toEye
)
{
    const float3 halfway = normalize(lightVec + toEye);
    
    const float hDotN = max(dot(halfway, normal), 0.f);
    
    return ambient + (diffuse + specular * pow(hDotN, shininess)) * lightStrength;
}

float4 main(PSBlinPhongInput input) : SV_TARGET
{
    float3 color = float3(0.f, 0.f, 0.f);
    
    const float3 toEye = cameraPos - input.posWorld;
    const float3 normal = normalize(input.normalWorld);
    
    // directional light
    {
        const float3 lightVec = normalize(-dir);
        
        const float nDotL = max(dot(normal, lightVec), 0.f);
        
        const float3 lightStrength = strength * nDotL;
        
        const float3 result = ComputeBlinnPhong(lightVec, lightStrength, input.posWorld, input.normalWorld, toEye);

        color += result;
    }
    
    [unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        float3 posToLight = lights[i].position - input.posWorld;
        
        const float dist = length(posToLight);

        if (dist > lights[i].fallOffEnd)
        {
            color += float3(0.f, 0.f, 0.f);
        }
        else
        {
            // point(fallstart == fallend), spot light
            
            posToLight /= dist;
            
            const float nDotL = max(dot(posToLight, normal), 0.f);
            float3 lightStrength = lights[i].strength * nDotL;

            const float attenuation = (lights[i].fallOffEnd - dist) / (lights[i].fallOffEnd - lights[i].fallOffStart);
            
            lightStrength *= attenuation;
            
            const float3 lightDir = normalize(lights[i].direction);
            const float spotFactor = pow(max(dot(-posToLight, lightDir), 0.f), lights[i].spotPower);

            lightStrength *= spotFactor;
            
            color += ComputeBlinnPhong(posToLight, lightStrength, input.posWorld, normal, toEye);
        }
    }
    
    const float4 resultColor = float4(saturate(color), 1.f);
    
    return bUseTexture ? sTexture.Sample(sSamplerState, input.uv) * resultColor : resultColor;
}