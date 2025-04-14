#include "DefaltLit.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallicMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D emissiveMap : register(t4);
Texture2D depthBuffer : register(t5);
SamplerState depthBufferSampler : register(s2);

SamplerState diffuseMapSamplerState : register(s0);
Texture2D environmentMap : register(t15);
SamplerState environmentMapSamplerState : register(s1);
SamplerState shadowmapSampler : register(s3);
Texture2D shadowmap : register(t10);

cbuffer CbShadowMap : register(b10)
{
    row_major float4x4 shadowProjection; // �V���h�E�v���W�F�N�V����
    float shadowBias; // �V���h�E�o�C�A�X
    float3 shadowPadding; // �V���h�E�̃p�f�B���O
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // �x�[�X�J���[�擾
    float4 baseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;

    // ���^���b�N�ƃ��t�l�X�擾
    float metallic = metallicMap.Sample(diffuseMapSamplerState, pin.texcoord).r;
    float roughness = roughnessMap.Sample(diffuseMapSamplerState, pin.texcoord).r;

    // �@���E�����E���C�g�v�Z
    float3 N = normalize(pin.normal);
    float3 V = normalize(-pin.worldPosition);
    float3 L = normalize(-lightDirection.xyz);
    float d = max(0.0, dot(N, L));
    float3 R = reflect(-L, N);

    // === �V���h�E�}�b�v�v�Z ===
    float4 shadowPos = mul(float4(pin.worldPosition, 1.0), shadowProjection);
    shadowPos.xyz /= shadowPos.w;
    shadowPos.xy = shadowPos.xy * 0.5 + 0.5;
    shadowPos.y = 1.0 - shadowPos.y;
    float shadowDepth = shadowmap.Sample(shadowmapSampler, shadowPos.xy).r;
    float currentDepth = shadowPos.z - shadowBias;
    float shadowFactor = (currentDepth > shadowDepth) ? 0.5 : 1.0;

    // === �������� (lightRange ��K�p) ===
    float distanceToLight = length(pin.worldPosition - lightPosition);
    float attenuation = saturate(1.0 - (distanceToLight / lightRange));
    attenuation *= attenuation; // ���Ō���������
    shadowFactor *= attenuation; // �V���h�E�̉e���ɂ��K�p

    // === �X�y�L�����[���v�Z ===
    float specPower = pow(saturate(dot(R, V)), 50.0 * (1.0 - roughness));
    float3 specularColor = lerp(float3(1.0, 1.0, 1.0), baseColor.rgb, metallic);
    float3 specular = specularColor * specPower * specularStrength;

    // === ���}�b�v�ɂ�锽�� ===
    float3 reflectedDir = normalize(reflect(-V, N));
    float2 envUV = float2(0.5, 0.5) + reflectedDir.xy * 0.5;
    float4 envColor = environmentMap.SampleLevel(environmentMapSamplerState, envUV, 0);
    envColor.rgb = envColor.rgb / (envColor.rgb + 1000.0);

    // === ���C�e�B���O�v�Z ===
    float3 ambient = ambientStrength * lightColor.rgb * baseColor.rgb;
    float3 diffuse = diffuseStrength * d * baseColor.rgb * lightColor.rgb;
    float3 finalSpecular = specular;
    float3 finalEmissive = emissiveColor.rgb * emissiveStrength;

    // === �t�H�O�ݒ� ===
    float distance = length(pin.worldPosition - lightPosition.xyz);
    float fogFactor = saturate((fogEnd - distance) / (fogEnd - fogStart));
    if (distance < 10.0)
        fogFactor = 1.0;
    float distanceFactor = saturate((distance - 5.0) / 50.0);
    float adjustedAmbientStrength = lerp(1.2, ambientStrength, distanceFactor);
    float adjustedSpecularStrength = lerp(0.8, specularStrength, distanceFactor);

    // ���C�e�B���O�␳�K�p
    ambient = adjustedAmbientStrength * lightColor.rgb * baseColor.rgb;
    finalSpecular = adjustedSpecularStrength * specular;

    // �ŏI�J���[�v�Z
    float3 litColor = (ambient + diffuse + finalSpecular) * attenuation + envColor.rgb;
    float3 emissive = finalEmissive; // materialColor �����Ȃ��I

    float3 colorSum = litColor * materialColor.rgb + emissive;
    float4 finalColor = float4(saturate(colorSum), materialColor.w); // alpha �͂��̂܂܂�OK
    finalColor.rgb *= shadowFactor;
    finalColor.rgb = lerp(fogColor.rgb, finalColor.rgb, fogFactor);

    return finalColor;
}