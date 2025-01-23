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
	void DrawingSettings(Graphics& graphics);

public:
	/*! デバッグ用 */
	static bool debugControll; // デバッグ用に数値を変更できるようにするか

	static DirectX::XMFLOAT4 debugLightColor;     // ライトの色
	static DirectX::XMFLOAT4 debugLightDirection; // ライトの向き
	static DirectX::XMFLOAT3 debugLightPosition;  // ライトの位置

	static float debugAmbientStrength; // 環境光
	static float debugDiffuseStrength; // 拡散光
	static float debugSpecularStrength; // スペキュラー光
	static float debugLightRange;       // ライトの範囲

	static DirectX::XMFLOAT3 debugFogColor; // フォグの色
	static float             debugFogStart;  // フォグの開始
	static float             debugFogEnd; // フォグの終了

protected:
	ID3D11DeviceContext* dc;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;
	RenderContext rc;

protected:

	DirectX::XMFLOAT3 lightColor = { 0.8f, 0.8f, 0.8f };  // ライトの色
	DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f }; // ライトの向き
	DirectX::XMFLOAT3 lightPosition = { 0, 0, 0 };           // ライトの位置
	float lightRange = 100.0f;

	DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f }; // クリアカラー

	float ambientStrength = 1.3f; // 環境光
	float diffuseStrength = 0.5f; // 拡散光
	float specularStrength = 0.5f; // スペキュラー光

	DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f }; // フォグの色
	float             fogStart = 80.0f;                // フォグの開始
	float             fogEnd = 200.0f;               // フォグの終了

	const float contrastConst = 1.0f;                  // コントラスト(数値固定)
	const float saturationConst = 0.8f;                  // 色の彩度(数値固定)
	const DirectX::XMFLOAT3 colorFilterConst = { 1.2f, 1.3f, 1.35f }; // カラーフィルター(数値固定)
	const float chromaticAberrationConst = 0.0f;                  // 色収差(数値固定)

private:
	bool ready = false;
};