#include "BlinnPhong.hlsli"

float4 main(const PSBlinnPhongInput input) : SV_Target
{
    float3 color = float3(0.f, 0.f, 0.f);
    
    const float3 toEye = normalize(cameraPos - input.posWorld);
    const float3 normal = normalize(input.normal);
    
    [unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        float3 posToLight = lights[i].position - input.posWorld;
        
        const float dist = length(posToLight);
        
        if (dist > lights[i].falloffEnd)
        {
            color += float3(0.f, 0.f, 0.f);
        }
        else
        {
            posToLight /= dist;
            
            const float nDotL = max(dot(posToLight, normal), 0.f);
            float3 lightStrength = lights[i].strength * nDotL;
            
            const float attenuation = (lights[i].falloffEnd - dist) / (lights[i].falloffEnd - lights[i].falloffStart);
            
            lightStrength *= attenuation;
            
            const float3 lightDir = normalize(lights[i].direction);
            const float spotFactor = pow(max(dot(-posToLight, lightDir), 0.f), lights[i].spotPower);
            
            lightStrength *= spotFactor;
            
            const float3 halfway = normalize(posToLight + toEye);
            
            const float hDotN = max(dot(halfway, normal), 0.f);
            
            color += ambientColor + (diffuseColor + specularColor * pow(hDotN, shininess)) * lightStrength;

        }
    }
    
    const float4 resultColor = float4(color, 1.f);
    
    return bUseTexture ? sTexture.Sample(sSampler, input.uv) * resultColor : resultColor;
}