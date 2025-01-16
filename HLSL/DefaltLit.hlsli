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

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection; // ライトの向き
    float4 lightColor; // ライトの色
    float3 lightPosition; // カメラの位置
    float lightRange; // ライトの範囲
    
    float ambientStrength; // 環境光
    float diffuseStrength; // 拡散光
    float specularStrength; // スペキュラー光
    float lightPadding; // ライトのパディング
    
    float4 fogColor; // フォグの色
    float fogStart; // フォグの開始
    float fogEnd; // フォグの終了
    float2 fogPadding; // フォグのパディング
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