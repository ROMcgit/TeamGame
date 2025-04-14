//PostEffectPS
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

Texture2D spriteTexture : register(t0);
Texture2D depthTexture : register(t1); // �[�x�e�N�X�`��
SamplerState spriteSampler : register(s0);

// �ϐ���`
cbuffer CBPostEffect : register(b5)
{
    float _contrast; // �R���g���X�g
    float _saturation; // �T�`�����[�V����
    float _chromatic_aberration; // �N���}�e�B�b�N�A�x���[�V����
    float _hueShift; // �F���V�t�g�i-1.0 �` 1.0�j

    float3 _colorFilter; // �J���[�t�B���^�[
    float dummy1; // 4��؂�p
    
    float _vignetteIntensity; // �r�l�b�g���x
    float _vignetteOpacity; // �r�l�b�g�̕s�����x
    float _blurStrength; // �u���[���x
    float dummy2; // 4��؂�p
    
    float _bloomThreshold; // �O���A��臒l
    float _bloomIntensity; // �O���A�̋��x
    float2 dummy3; // 4��؂�p
    
    float _focusDistance; // �s���g�����킹�鋗��
    float _focusRange; // �s���g�������͈�
    float _dofBlurStrength; // ��ʊE�[�x�̃u���[���x
    float dummy4; // 4��؂�p
    
    float2 _shakeOffset; // �V�F�C�N�̃I�t�Z�b�g
    float _shakeStrength; // �V�F�C�N�̋��x
    float _shakeTime; // �O������n������
    float _shakeSpeed; // �V�F�C�N�̑��x
    float3 dummy5; // 4��؂�p
}

// �R���g���X�g
float3 Contrast(float3 color)
{
    color -= 0.5f; // ��������ǂꂭ�炢�Â����邢�H
    color *= _contrast; // ��������̖��Â�contrast�{

    color += 0.5f;
    return color;
}

// �ʓx
float3 Saturation(float3 color)
{
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
    color *= _colorFilter;
    return color;
}

// �N���}�e�B�b�N�A�x���[�V����
float3 ChromaticAberration(float2 uv)
{
    // ���S����̍����l�������Y��
    float2 v = (0.5 - uv) * _chromatic_aberration; //���炷����
    float r = spriteTexture.Sample(spriteSampler, uv).r;
    float g = spriteTexture.Sample(spriteSampler, uv + v).g;
    float b = spriteTexture.Sample(spriteSampler, uv + v * 2.0).b;

    return float3(r, g, b);
}

//-------------------------------------------------------------//

// RGB -> HSV
float3 RGBtoHSV(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// HSV -> RGB
float3 HSVtoRGB(float3 hsv)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(hsv.xxx + K.xyz) * 6.0 - K.www);
    return hsv.z * lerp(K.xxx, saturate(p - K.xxx), hsv.y);
}

// �F���V�t�g
float3 HueShift(float3 color)
{
    float3 hsv = RGBtoHSV(color);
    hsv.x = frac(hsv.x + _hueShift); // �F�����V�t�g
    return HSVtoRGB(hsv);
}

//-------------------------------------------------------------//

// �r�l�b�g�G�t�F�N�g
float3 Vignette(float3 color, float2 uv)
{
    if (_vignetteIntensity > 0)
    {
        float2 center = float2(0.5, 0.5);
        float dist = length(uv - center);
        float vignette = 1.0 - smoothstep(0.5 - _vignetteIntensity, 0.5, dist);
        
        // _vignetteOpacity �Ńr�l�b�g�̈Â��𒲐��i1.0 �Ȃ犮�S�ɍ����Ȃ�j
        vignette = lerp(1.0, vignette, _vignetteOpacity);
        
        color *= vignette;
    }
    return color;
}

// �K�E�X�u���[
float3 GaussianBlur(float2 uv, float blurStrength)
{
    float2 texelSize = float2(1.0 / 1920.0, 1.0 / 1080.0);
    float3 sum = float3(0, 0, 0);
    float weight[5] = { 0.227, 0.194, 0.121, 0.054, 0.016 };
    for (int i = -2; i <= 2; i++)
    {
        sum += spriteTexture.Sample(spriteSampler, uv + float2(i, 0) * texelSize * blurStrength).rgb * weight[abs(i)];
    }
    return sum;
}

// ��ʊE�[�x�iDepth of Field�j
float3 DepthOfField(float2 uv)
{
    float depth = depthTexture.Sample(spriteSampler, uv).r; // �[�x�擾
    float blurFactor = abs(depth - _focusDistance) / _focusRange;
    blurFactor = saturate(blurFactor) * _dofBlurStrength * _blurStrength;
    return GaussianBlur(uv, blurFactor);
}

// �O���A�iBloom�j
float3 BloomEffect(float2 uv)
{
    float3 color = spriteTexture.Sample(spriteSampler, uv).rgb;
    float brightness = dot(color, float3(0.2126, 0.7152, 0.0722));
    if (brightness < _bloomThreshold)
        return float3(0, 0, 0);
    return GaussianBlur(uv, _blurStrength) * _bloomIntensity;
}

// ��ʗh��i�V�F�C�N�j
float2 ScreenShake(float2 uv)
{
    float shakeX = sin(_shakeTime * _shakeSpeed) * _shakeStrength;
    float shakeY = cos(_shakeTime * _shakeSpeed) * _shakeStrength;
    float2 shake = float2(shakeX, shakeY) * _shakeOffset;
    uv += shake;

    // UV���W��[0, 1]�͈͂ɐ���
    uv = clamp(uv, 0.0f, 1.0f);
    
    return uv;
}

// �s�N�Z���V�F�[�_�[�G���g���|�C���g
float4 main(VS_OUT pin) : SV_TARGET
{
    float2 shakenUV = ScreenShake(pin.texcoord); // �V�F�C�N�K�p

    float3 color = ChromaticAberration(shakenUV);
    color = DepthOfField(pin.texcoord) * 0.5 + color * 0.5;
    color = Contrast(color);
    color = Saturation(color);
    color = ColorFilter(color);
    color = HueShift(color);
    color = Vignette(color, pin.texcoord);
    
    float3 bloom = BloomEffect(pin.texcoord);
    color += bloom;
    
    return float4(color, 1.0f);
}
