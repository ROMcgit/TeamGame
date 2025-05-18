//ShadowMap.hlsli

// シャドウマップ用コンスタントバッファ
cbuffer CbShadowMap : register(b10)
{
    row_major float4x4 shadowProjection;
}

// 頂点シェーダー　→　ピクセルシェーダーに渡す値
struct VS_OUT
{
    float4 position : SV_POSITION;
    float distance : TEXCOORD0;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};