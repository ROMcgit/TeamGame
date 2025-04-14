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

	static DirectX::XMFLOAT4 debugLightColor;         // ライトの色
	static DirectX::XMFLOAT4 debugLightDirection;     // ライトの向き
	static DirectX::XMFLOAT3 debugLightPosition;      // ライトの位置
	static DirectX::XMFLOAT3 debugShadowMapEyeOffset; // シャドウマップの位置のオフセット
	static float             debugShadowMapBias;      // シャドウマップのバイアス
	static float             debugShadowMapWidth;     // シャドウマップの横の範囲
	static float             debugShadowMapLength;    // シャドウマップの縦の範囲
	static float             debugShadowMapStart;     // シャドウマップの開始
	static float             debugShadowMapEnd;       // シャドウマップの終了

	static float debugAmbientStrength; // 環境光
	static float debugDiffuseStrength; // 拡散光
	static float debugSpecularStrength; // スペキュラー光
	static float debugLightRange;       // ライトの範囲

	static DirectX::XMFLOAT3 debugFogColor; // フォグの色
	static float             debugFogStart; // フォグの開始
	static float             debugFogEnd;   // フォグの終了

protected:
	ID3D11DeviceContext* dc; // デバイスコンテキスト
	ID3D11RenderTargetView* rtv; // レンダーターゲットビュー
	ID3D11DepthStencilView* dsv; // デプスステンシルビュー
	RenderContext            rc; // レンダーコンテキスト

public:
	/*! シャドウマップクラスで使う */
	static DirectX::XMFLOAT3 shadowMapEyeOffsetStatic; // シャドウマップの位置のオフセット
	static float shadowMapBiasStatic;                  // シャドウマップのバイアス
	static float shadowMapWidthStatic;                 // シャドウマップの横の範囲
	static float shadowMapLengthStatic;                // シャドウマップの縦の範囲
	static float shadowMapStartStatic;                 // シャドウマップの開始
	static float shadowMapEndStatic;                   // シャドウマップの終了

protected:

	DirectX::XMFLOAT3 lightColor = { 0.8f, 0.8f, 0.8f };  // ライトの色
	DirectX::XMFLOAT3 lightDirection = { 0.3f, -1.0f, 0.0f }; // ライトの向き
	DirectX::XMFLOAT3 lightPosition = { 0.0f, 0.0f, 0.0f };  // ライトの位置
	float             lightRange = 2000.0f;               // ライトの範囲
	DirectX::XMFLOAT3 shadowMapEyeOffset = { -1.7f, 30.0f, -9.5f };  // シャドウマップの位置のオフセット
	float             shadowMapBias = 0.005f;                // シャドウマップのバイアス
	float             shadowMapWidth = 100.0f;                // シャドウマップの横の範囲
	float             shadowMapLength = 100.0f;                // シャドウマップの縦の範囲
	float             shadowMapStart = 1.0f;                  // シャドウマップの開始
	float             shadowMapEnd = 100.0f;                // シャドウマップの終了

	DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f }; // クリアカラー

	float ambientStrength = 1.3f; // 環境光
	float diffuseStrength = 0.5f; // 拡散光
	float specularStrength = 0.5f; // スペキュラー光

	DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f }; // フォグの色
	float             fogStart = 80.0f;                // フォグの開始
	float             fogEnd = 200.0f;               // フォグの終了

public:
	static float             contrastStatic;            // コントラスト
	static float             saturationStatic;          // 色の彩度(数値固定)
	static DirectX::XMFLOAT3 colorFilterStatic;         // カラーフィルター(数値固定)
	static float             chromaticAberrationStatic; // 色収差(数値固定)
	static float             hueShiftStatic;            // 色相シフト

	static float vignetteIntensityStatic; // ビネットの強度
	static float vignetteOpacityStatic;   // ビネットの不透明度
	static float blurStrengthStatic;      // ブラーの強度
	static float bloomThresholdStatic;    // グレアの閾値
	static float bloomIntensityStatic;    // グレアの強度

	static float focusDistanceStatic;     // ピントを合わせる距離
	static float focusRangeStatic;        // ピントが合う範囲
	static float dofBlurStrengthStatic;   // 被写界深度のブラー強度

protected:
	float contrast = 1.0f;                  // コントラスト
	float saturation = 0.8f;                  // 色の彩度
	DirectX::XMFLOAT3 colorFilter = { 1.2f, 1.3f, 1.35f }; // カラーフィルター
	float chromaticAberration = 0.0f;                  // 色収差
	float hueShift = 0.0f;                  // 色相シフト

	float vignetteIntensity = 0.0f; // ビネットの強度
	float vignetteOpacity = 0.0f; // ビネットの不透明度
	float blurStrength = 0.0f; // ブラーの強度
	float bloomThreshold = 1.3f; // グレアの閾値
	float bloomIntensity = 1.0f; // グレアの強度

	float focusDistance = 0.3f;  // ピントを合わせる距離
	float focusRange = 3.0f;  // ピントが合う範囲
	float dofBlurStrength = 15.0f; // 被写界深度のブラー強度

private:
	bool ready = false;
};