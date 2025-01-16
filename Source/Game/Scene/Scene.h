#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Graphics/Graphics.h"

// シーン
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// 初期化
	virtual void Initialize() = 0;

	// 終了化
	virtual void Finalize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render() = 0;

	// 準備完了しているか
	bool IsReady() const { return ready; }

	// 準備完了設定
	void SetReady() { ready = true; }

	// 描画設定
	void DrawingSettings(Graphics& graphics,
		DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f },
		float ambientStrength = 1.3f,
		float diffuseStrength = 0.5f,
		float specularStrength = 0.5f,
		float lightRange = 100.0f,
		DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f },
		float fogStart = 80.0f, float fogEnd = 200.0f);

public:
	static DirectX::XMFLOAT4 lightDirection; // ライトの向き
	static DirectX::XMFLOAT4 lightColor;     // ライトの色
	static DirectX::XMFLOAT3 lightPosition;  // ライトの位置

protected:
	ID3D11DeviceContext* dc;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;
	RenderContext rc;

protected:
	DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f }; // クリアカラー
	float ambientStrength = 1.3f; // 環境光
	float diffuseStrength = 0.5f; // 拡散光
	float specularStrength = 0.5f; // スペキュラー光
	float lightRange = 1.0f; // ライトの範囲

	DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f }; // フォグの色
	float fogStart = 80.0f;  // フォグの開始
	float fogEnd = 200.0f; // フォグの終了

private:
	bool ready = false;
};