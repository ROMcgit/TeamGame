//skydomePS
#include "Skydome.hlsli"

// IBL用環境マップを使いまわす
Texture2D environment_map : register(t15); // Slot15 環境マップ
SamplerState environmentMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord + float2(0.5, 0.0);
    float4 tex = environment_map.SampleLevel(environmentMapSamplerState, uv, 0);
    // トーンマップ
    tex.rgb = tex.rgb / (tex.rgb + 500) * 2.5;
    
    return tex;
}