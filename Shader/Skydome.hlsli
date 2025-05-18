#include "Scene.hlsli"

// 頂点シェーダー　→　ピクセルシェーダーに渡す値
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// 大きさ
cbuffer SkydomeTransform
{
    float3 skydomePosition;
    float  dummy;
    float3 skydomeAngle;
    float  skydomeScale;
};