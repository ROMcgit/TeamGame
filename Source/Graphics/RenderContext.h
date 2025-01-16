#pragma once

#include <DirectXMath.h>

// レンダーコンテキスト
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	DirectX::XMFLOAT4X4	 view;
	DirectX::XMFLOAT4X4	 projection;
	DirectX::XMFLOAT4	 lightDirection;   // ライトの向き
	DirectX::XMFLOAT4    lightColor;       // ライトの色
	DirectX::XMFLOAT3    lightPosition;    // ライトの位置
	float                lightRange;       // ライトの範囲

	float                ambientStrength;  // 環境光
	float                diffuseStrength;  // 拡散光
	float                specularStrength; // スペキュラー光

	DirectX::XMFLOAT4    fogColor;         // フォグの色
	float                fogStart;         // フォグの開始
	float                fogEnd;           // フォグの終了

	float                focusDistance; // フォーカス距離
	float                focusRange;    // フォーカス範囲
	float                blurStrength;  // ぼかし強度
};
