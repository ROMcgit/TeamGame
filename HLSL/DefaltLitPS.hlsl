#include "DefaltLit.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1); // �m�[�}���}�b�v
Texture2D metallicMap : register(t2); // ���^���b�N�l
Texture2D roughnessMap : register(t3); // ���t�l�X�}�b�v
Texture2D emissiveMap : register(t4); // �G�~�b�V�u�}�b�v
Texture2D depthBuffer : register(t5); // �[�x�o�b�t�@
SamplerState depthBufferSampler : register(s2);

SamplerState diffuseMapSamplerState : register(s0);

Texture2D environmentMap : register(t15); // ���}�b�v
SamplerState environmentMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // �x�[�X�J���[�擾
    float4 baseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;

    // ���^���b�N�ƃ��t�l�X�擾
    float metallic = metallicMap.Sample(diffuseMapSamplerState, pin.texcoord).r;
    float roughness = roughnessMap.Sample(diffuseMapSamplerState, pin.texcoord).r;

    // �@���E�����E���C�g�v�Z
    float3 N = normalize(pin.normal);
    float3 V = normalize(-pin.worldPosition); // ��������
    float3 L = normalize(-lightDirection.xyz); // ���C�g����
    float d = max(0.0, dot(N, L)); // �g�U��
    float3 R = reflect(-L, N); // ���˃x�N�g��

//------------------------------------------------------------------------------------//
    
    // === �X�y�L�����[���v�Z ===
    // ���t�l�X�Ńn�C���C�g�̍L����𐧌�
    float specPower = pow(saturate(dot(R, V)), 50.0 * (1.0 - roughness));

    // ���^���b�N�ŃX�y�L�����[���̐F�𐧌�
    float3 specularColor = lerp(float3(1.0, 1.0, 1.0), baseColor.rgb, metallic);
    float3 specular = specularColor * specPower * specularStrength;

//------------------------------------------------------------------------------------//
    
    // === ���}�b�v�ɂ�锽�� ===
    float3 reflectedDir = normalize(reflect(-V, N));
    float2 envUV = float2(0.5, 0.5) + reflectedDir.xy * 0.5;
    float4 envColor = environmentMap.SampleLevel(environmentMapSamplerState, envUV, 0);
    envColor.rgb = envColor.rgb / (envColor.rgb + 1000.0);
    
    // === ���C�e�B���O�v�Z ===
    // �����A�g�U���A�X�y�L�����[��
    float3 ambient = ambientStrength * lightColor.rgb * baseColor.rgb;
    float3 diffuse = diffuseStrength * d * baseColor.rgb * lightColor.rgb;
    float3 finalSpecular = specular;

    // �G�~�b�V�u����
    float3 finalEmissive = emissiveColor.rgb * emissiveStrength;

//------------------------------------------------------------------------------------//
    
    // �t�H�O�ݒ�
    float distance = length(pin.worldPosition - lightPosition.xyz);
    float fogFactor = saturate((fogEnd - distance) / (fogEnd - fogStart));

    // === �ǉ�����: �����ɂ��␳ ===
    if (distance < 10.0)
    {
        fogFactor = 1.0; // �ߋ����ł̓t�H�O������
    }
    float distanceFactor = saturate((distance - 5.0) / 50.0); // �����ɂ��␳
    float adjustedAmbientStrength = lerp(1.2, ambientStrength, distanceFactor);
    float adjustedSpecularStrength = lerp(0.8, specularStrength, distanceFactor);

    // ���C�e�B���O�␳��K�p
    ambient = adjustedAmbientStrength * lightColor.rgb * baseColor.rgb;
    finalSpecular = adjustedSpecularStrength * specular;

    // ���C�e�B���O���ʂ̍��v
    float3 colorSum = ambient + diffuse + finalSpecular + envColor.rgb + finalEmissive;
    
//------------------------------------------------------------------------------------//
    
    // �ŏI�J���[
    float4 finalColor = float4(saturate(colorSum * materialColor.rgb), materialColor.w);

    // �t�H�O��K�p
    finalColor.rgb = lerp(fogColor.rgb, finalColor.rgb, fogFactor);
    
    return finalColor;
}