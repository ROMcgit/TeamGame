cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    
    float4 cameraPosition; // カメラの位置
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