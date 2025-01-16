#include "DefaltLit.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1); // ノーマルマップ
Texture2D metallicMap : register(t2); // メタリック値
Texture2D roughnessMap : register(t3); // ラフネスマップ
Texture2D emissiveMap : register(t4); // エミッシブマップ
Texture2D depthBuffer : register(t5); // 深度バッファ
SamplerState depthBufferSampler : register(s2);

SamplerState diffuseMapSamplerState : register(s0);

Texture2D environmentMap : register(t15); // 環境マップ
SamplerState environmentMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // ベースカラー取得
    float4 baseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;

    // メタリックとラフネス取得
    float metallic = metallicMap.Sample(diffuseMapSamplerState, pin.texcoord).r;
    float roughness = roughnessMap.Sample(diffuseMapSamplerState, pin.texcoord).r;

    // 法線・視線・ライト計算
    float3 N = normalize(pin.normal);
    float3 V = normalize(-pin.worldPosition); // 視線方向
    float3 L = normalize(-lightDirection.xyz); // ライト方向
    float d = max(0.0, dot(N, L)); // 拡散光
    float3 R = reflect(-L, N); // 反射ベクトル

//------------------------------------------------------------------------------------//
    
    // === スペキュラー光計算 ===
    // ラフネスでハイライトの広がりを制御
    float specPower = pow(saturate(dot(R, V)), 50.0 * (1.0 - roughness));

    // メタリックでスペキュラー光の色を制御
    float3 specularColor = lerp(float3(1.0, 1.0, 1.0), baseColor.rgb, metallic);
    float3 specular = specularColor * specPower * specularStrength;

//------------------------------------------------------------------------------------//
    
    // === 環境マップによる反射 ===
    float3 reflectedDir = normalize(reflect(-V, N));
    float2 envUV = float2(0.5, 0.5) + reflectedDir.xy * 0.5;
    float4 envColor = environmentMap.SampleLevel(environmentMapSamplerState, envUV, 0);
    envColor.rgb = envColor.rgb / (envColor.rgb + 1000.0);
    
    // === ライティング計算 ===
    // 環境光、拡散光、スペキュラー光
    float3 ambient = ambientStrength * lightColor.rgb * baseColor.rgb;
    float3 diffuse = diffuseStrength * d * baseColor.rgb * lightColor.rgb;
    float3 finalSpecular = specular;

    // エミッシブ成分
    float3 finalEmissive = emissiveColor.rgb * emissiveStrength;

//------------------------------------------------------------------------------------//
    
    // フォグ設定
    float distance = length(pin.worldPosition - lightPosition.xyz);
    float fogFactor = saturate((fogEnd - distance) / (fogEnd - fogStart));

    // === 追加処理: 距離による補正 ===
    if (distance < 10.0)
    {
        fogFactor = 1.0; // 近距離ではフォグ無効化
    }
    float distanceFactor = saturate((distance - 5.0) / 50.0); // 距離による補正
    float adjustedAmbientStrength = lerp(1.2, ambientStrength, distanceFactor);
    float adjustedSpecularStrength = lerp(0.8, specularStrength, distanceFactor);

    // ライティング補正を適用
    ambient = adjustedAmbientStrength * lightColor.rgb * baseColor.rgb;
    finalSpecular = adjustedSpecularStrength * specular;

    // ライティング結果の合計
    float3 colorSum = ambient + diffuse + finalSpecular + envColor.rgb + finalEmissive;
    
//------------------------------------------------------------------------------------//
    
    // 最終カラー
    float4 finalColor = float4(saturate(colorSum * materialColor.rgb), materialColor.w);

    // フォグを適用
    finalColor.rgb = lerp(fogColor.rgb, finalColor.rgb, fogFactor);
    
    return finalColor;
}