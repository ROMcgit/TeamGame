#include "Scene.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL; // 法線
    float3 worldPosition : POSITION; // ワールド座標
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
    float3 emissiveColor;
    float emissiveStrength;
};