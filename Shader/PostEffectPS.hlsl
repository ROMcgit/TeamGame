//PostEffectPS
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

// 変数定義
cbuffer CBPostEffect : register(b5)
{
    float _contrast;
    float _saturation;
    float _chromatic_aberration;
    float dummy; //4個区切り用

    float3 _ColorFilter;
    float dummy2; //4個区切り用
}


// コントラスト
float3 Contrast(float3 color)
{
   // float _contrast = 2.0f;
    color -= 0.5f; // 中央からどれくらい暗い明るい？
    color *= _contrast; //中央からの明暗をcontrast倍

    color += 0.5f;
    return color;
}

// 彩度
float3 Saturation(float3 color)
{
    //float _saturation = 0.5f;
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
    //float3 _ColorFilter = { 0.9, 1.0, 1.05 };
    color *= _ColorFilter;
    return color;
}

// クロマティックアベレーション
float3 ChromaticAberration(float2 uv)
{
    //float2 v = { 0.01, 0.01 }; // ずらす方向
    //float _chromatic_aberration = 0.02;
    // 中心からの差を考慮したズレ
    float2 v = (0.5 - uv) * _chromatic_aberration; //ずらす方向
    float r = spriteTexture.Sample(spriteSampler, uv).r;
    float g = spriteTexture.Sample(spriteSampler, uv + v).g;
    float b = spriteTexture.Sample(spriteSampler, uv + v * 2.0).b;

    return float3(r, g, b);
}

// ピクセルシェーダーエントリポイント
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 tex; // = spriteTexture.Sample(spriteSampler, pin.texcoord);
    tex.rgb = ChromaticAberration(pin.texcoord);
    tex.a = 1.0f;
    
    float3 color = tex.rgb;
    color = Contrast(color);
    color = Saturation(color);
    color = ColorFilter(color);
    //color.r = 1; 確認用
    return float4(color, 1.0f);
}
