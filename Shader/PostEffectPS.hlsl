//PostEffectPS
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

// �ϐ���`
cbuffer CBPostEffect : register(b5)
{
    float _contrast;
    float _saturation;
    float _chromatic_aberration;
    float dummy; //4��؂�p

    float3 _ColorFilter;
    float dummy2; //4��؂�p
}


// �R���g���X�g
float3 Contrast(float3 color)
{
   // float _contrast = 2.0f;
    color -= 0.5f; // ��������ǂꂭ�炢�Â����邢�H
    color *= _contrast; //��������̖��Â�contrast�{

    color += 0.5f;
    return color;
}

// �ʓx
float3 Saturation(float3 color)
{
    //float _saturation = 0.5f;
    float gray = 0.299 * color.r 
               + 0.587 * color.g 
               + 0.114 * color.b;
    
    color -= gray; // �O���[����́}
    color *= _saturation;
    color += gray;
    
    return color;
}

// �J���[�t�B���^�[
float3 ColorFilter(float3 color)
{
    //float3 _ColorFilter = { 0.9, 1.0, 1.05 };
    color *= _ColorFilter;
    return color;
}

// �N���}�e�B�b�N�A�x���[�V����
float3 ChromaticAberration(float2 uv)
{
    //float2 v = { 0.01, 0.01 }; // ���炷����
    //float _chromatic_aberration = 0.02;
    // ���S����̍����l�������Y��
    float2 v = (0.5 - uv) * _chromatic_aberration; //���炷����
    float r = spriteTexture.Sample(spriteSampler, uv).r;
    float g = spriteTexture.Sample(spriteSampler, uv + v).g;
    float b = spriteTexture.Sample(spriteSampler, uv + v * 2.0).b;

    return float3(r, g, b);
}

// �s�N�Z���V�F�[�_�[�G���g���|�C���g
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 tex; // = spriteTexture.Sample(spriteSampler, pin.texcoord);
    tex.rgb = ChromaticAberration(pin.texcoord);
    tex.a = 1.0f;
    
    float3 color = tex.rgb;
    color = Contrast(color);
    color = Saturation(color);
    color = ColorFilter(color);
    //color.r = 1; �m�F�p
    return float4(color, 1.0f);
}
