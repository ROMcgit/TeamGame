//PostEffectPS
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

Texture2D spriteTexture : register(t0);
Texture2D depthTexture : register(t1); // 深度テクスチャ
SamplerState spriteSampler : register(s0);

// 変数定義
cbuffer CBPostEffect : register(b5)
{
    float _contrast; // コントラスト
    float _saturation; // サチュレーション
    float _chromatic_aberration; // クロマティックアベレーション
    float _hueShift; // 色相シフト（-1.0 ～ 1.0）

    float3 _colorFilter; // カラーフィルター
    float dummy1; // 4個区切り用
    
    float _vignetteIntensity; // ビネット強度
    float _vignetteOpacity; // ビネットの不透明度
    float _blurStrength; // ブラー強度
    float dummy2; // 4個区切り用
    
    float _bloomThreshold; // グレアの閾値
    float _bloomIntensity; // グレアの強度
    float2 dummy3; // 4個区切り用
    
    float _focusDistance; // ピントを合わせる距離
    float _focusRange; // ピントが合う範囲
    float _dofBlurStrength; // 被写界深度のブラー強度
    float dummy4; // 4個区切り用
    
    float2 _shakeOffset; // シェイクのオフセット
    float _shakeStrength; // シェイクの強度
    float _shakeTime; // 外部から渡す時間
    float _shakeSpeed; // シェイクの速度
    float3 dummy5; // 4個区切り用
}

// コントラスト
float3 Contrast(float3 color)
{
    color -= 0.5f; // 中央からどれくらい暗い明るい？
    color *= _contrast; // 中央からの明暗をcontrast倍

    color += 0.5f;
    return color;
}

// 彩度
float3 Saturation(float3 color)
{
    float gray = 0.299 * color.r 
               + 0.587 * color.g 
               + 0.114 * color.b;
    
    color -= gray; // グレーからの±
    color *= _saturation;
    color += gray;
    
    return color;
}

// カラーフィルター
float3 ColorFilter(float3 color)
{
    color *= _colorFilter;
    return color;
}

// クロマティックアベレーション
float3 ChromaticAberration(float2 uv)
{
    // 中心からの差を考慮したズレ
    float2 v = (0.5 - uv) * _chromatic_aberration; //ずらす方向
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

// 色相シフト
float3 HueShift(float3 color)
{
    float3 hsv = RGBtoHSV(color);
    hsv.x = frac(hsv.x + _hueShift); // 色相をシフト
    return HSVtoRGB(hsv);
}

//-------------------------------------------------------------//

// ビネットエフェクト
float3 Vignette(float3 color, float2 uv)
{
    if (_vignetteIntensity > 0)
    {
        float2 center = float2(0.5, 0.5);
        float dist = length(uv - center);
        float vignette = 1.0 - smoothstep(0.5 - _vignetteIntensity, 0.5, dist);
        
        // _vignetteOpacity でビネットの暗さを調整（1.0 なら完全に黒くなる）
        vignette = lerp(1.0, vignette, _vignetteOpacity);
        
        color *= vignette;
    }
    return color;
}

// ガウスブラー
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

// 被写界深度（Depth of Field）
float3 DepthOfField(float2 uv)
{
    float depth = depthTexture.Sample(spriteSampler, uv).r; // 深度取得
    float blurFactor = abs(depth - _focusDistance) / _focusRange;
    blurFactor = saturate(blurFactor) * _dofBlurStrength * _blurStrength;
    return GaussianBlur(uv, blurFactor);
}

// グレア（Bloom）
float3 BloomEffect(float2 uv)
{
    float3 color = spriteTexture.Sample(spriteSampler, uv).rgb;
    float brightness = dot(color, float3(0.2126, 0.7152, 0.0722));
    if (brightness < _bloomThreshold)
        return float3(0, 0, 0);
    return GaussianBlur(uv, _blurStrength) * _bloomIntensity;
}

// 画面揺れ（シェイク）
float2 ScreenShake(float2 uv)
{
    float shakeX = sin(_shakeTime * _shakeSpeed) * _shakeStrength;
    float shakeY = cos(_shakeTime * _shakeSpeed) * _shakeStrength;
    float2 shake = float2(shakeX, shakeY) * _shakeOffset;
    uv += shake;

    // UV座標を[0, 1]範囲に制限
    uv = clamp(uv, 0.0f, 1.0f);
    
    return uv;
}

// ピクセルシェーダーエントリポイント
float4 main(VS_OUT pin) : SV_TARGET
{
    float2 shakenUV = ScreenShake(pin.texcoord); // シェイク適用

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
