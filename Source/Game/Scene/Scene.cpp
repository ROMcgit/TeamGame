#include "Scene.h"
#include "Game/Camera/Camera.h"

/*! デバッグ用 */
bool Scene::debugControll = false;                       // デバッグ用に数値を変更できるようにするか
//! ライト
DirectX::XMFLOAT4 Scene::debugLightDirection = { 0.0f, -1.0f, 0.0f, 0.0f }; // ライトの向き
DirectX::XMFLOAT4 Scene::debugLightColor = { 0.8f, 0.8f, 0.8f, 1 };     // ライトの色
DirectX::XMFLOAT3 Scene::debugLightPosition = { 0.0f, 0.0f, 0.0f };        // ライトの位置

//! シャドウマップ
DirectX::XMFLOAT3 Scene::debugShadowMapEyeOffset = { 0.0f, 0.0f, 0.0f }; //! シャドウマップの位置のオフセット
float Scene::debugShadowMapBias = 0;                                    // シャドウマップのバイアス
float Scene::debugShadowMapWidth = 0;                                    // シャドウマップの横の範囲
float Scene::debugShadowMapLength = 0;                                    // シャドウマップの縦の範囲
float Scene::debugShadowMapStart = 0;                                    // シャドウマップの開始
float Scene::debugShadowMapEnd = 0;                                    // シャドウマップの終了

//! シャドウマップクラスで使う
DirectX::XMFLOAT3 Scene::shadowMapEyeOffsetStatic = { 0.0f, 0.0f, 0.0f }; //! シャドウマップの位置のオフセット
float Scene::shadowMapBiasStatic = 0;                                   // シャドウマップのバイアス
float Scene::shadowMapWidthStatic = 0;                                   // シャドウマップの横の範囲
float Scene::shadowMapLengthStatic = 0;                                   // シャドウマップの縦の範囲
float Scene::shadowMapStartStatic = 0;                                   // シャドウマップの開始
float Scene::shadowMapEndStatic = 0;                                   // シャドウマップの終了

//! ライトへの影響
float Scene::debugAmbientStrength = 1.3f; // 環境光
float Scene::debugDiffuseStrength = 0.5f; // 拡散光
float Scene::debugSpecularStrength = 0.5f; // スペキュラー光
float Scene::debugLightRange = 1.0f; // ライトの範囲

//! フォグ
DirectX::XMFLOAT3 Scene::debugFogColor = { 0.7f, 0.8f, 0.9f }; // フォグの色
float             Scene::debugFogStart = 80.0f;                // フォグの開始
float             Scene::debugFogEnd = 200.0f;               // フォグの終了

/*! ポストエフェクト */
//! コントラスト
float Scene::contrastStatic = 0.0f;
//! サチュレーション
float Scene::saturationStatic = 0.0f;
//! カラーフィルター
DirectX::XMFLOAT3 Scene::colorFilterStatic = { 0.0f, 0.0f, 0.0f };
//! クロマティックアベレーション
float Scene::chromaticAberrationStatic = 0.0f;
//! 色相シフト
float Scene::hueShiftStatic = 0.0f;

//! ビネットの強度
float Scene::vignetteIntensityStatic = 0.0f;
//! ビネットの不透明度
float Scene::vignetteOpacityStatic = 0.0f;
//! ブラーの強度
float Scene::blurStrengthStatic = 0.0f;
//! グレアの閾値
float Scene::bloomThresholdStatic = 0.0f;
//! グレアの強度
float Scene::bloomIntensityStatic = 0.0f;

//! ピントを合わせる距離
float Scene::focusDistanceStatic = 0.0f;
//! ピントが合う範囲
float Scene::focusRangeStatic = 0.0f;
//! 被写界深度のブラー強度
float Scene::dofBlurStrengthStatic = 0.0f;

// 描画設定
void Scene::DrawingSettings(Graphics& graphics)
{
	dc = graphics.GetDeviceContext();
	rtv = graphics.GetRenderTargetView();
	dsv = graphics.GetDepthStencilView();

	// 画面クリア&レンダーターゲット設定
	FLOAT color[] = { clearColor.x, clearColor.y, clearColor.z, 1.0f }; //RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	if (!debugControll)
	{
		/*! ライトの設定 */
		// ライトの色
		rc.lightColor = this->debugLightColor = { this->lightColor.x, this->lightColor.y, this->lightColor.z, 1.0f };
		// ライトの向き
		rc.lightDirection = this->debugLightDirection = { this->lightDirection.x, this->lightDirection.y, this->lightDirection.z, 1.0f };
		// ライトの位置
		rc.lightPosition = this->debugLightPosition = this->lightPosition;
		// ライトの範囲
		rc.lightRange = this->debugLightRange = this->lightRange;

		//! シャドウマップ
		// シャドウマップの位置のオフセット
		shadowMapEyeOffsetStatic = this->debugShadowMapEyeOffset = this->shadowMapEyeOffset;
		// シャドウマップのバイアス
		shadowMapBiasStatic = this->debugShadowMapBias = this->shadowMapBias;
		// シャドウマップの横の範囲
		shadowMapWidthStatic = this->debugShadowMapWidth = this->shadowMapWidth;
		// シャドウマップの縦の範囲
		shadowMapLengthStatic = this->debugShadowMapLength = this->shadowMapLength;
		// シャドウマップの開始
		shadowMapStartStatic = this->debugShadowMapStart = this->shadowMapStart;
		// シャドウマップの終了
		shadowMapEndStatic = this->debugShadowMapEnd = this->shadowMapEnd;

		// 環境光、拡散光、 スペキュラー光の設定
		rc.ambientStrength = this->debugAmbientStrength = this->ambientStrength;  // 環境光
		rc.diffuseStrength = this->debugDiffuseStrength = this->diffuseStrength;  // 拡散光
		rc.specularStrength = this->debugSpecularStrength = this->specularStrength; // スペキュラー光

		/*! フォグの設定 */
		// フォグの色
		rc.fogColor = { this->fogColor.x, this->fogColor.y, this->fogColor.z, 1.0f };
		// フォグの色(デバッグ)
		this->debugFogColor = { this->fogColor.x, this->fogColor.y, this->fogColor.z };
		// フォグの開始
		rc.fogStart = this->debugFogStart = this->fogStart;
		// フォグの終了
		rc.fogEnd = this->debugFogEnd = this->fogEnd;


		/*! ポストエフェクト */
		// コントラスト
		contrastStatic = this->contrast;
		// サチュレーション
		saturationStatic = this->saturation;
		// カラーフィルター
		colorFilterStatic = this->colorFilter;
		// クロマティックアベレーション
		chromaticAberrationStatic = this->chromaticAberration;
		// 色相シフト
		hueShiftStatic = this->hueShift;

		// ビネットの強度
		vignetteIntensityStatic = this->vignetteIntensity;
		// ビネットの不透明度
		vignetteOpacityStatic = this->vignetteOpacity;
		// ブラーの強度
		blurStrengthStatic = this->blurStrength;
		// グレアの閾値
		bloomThresholdStatic = this->bloomThreshold;
		// グレアの強度
		bloomIntensityStatic = this->bloomIntensity;

		// ピントを合わせる距離
		focusDistanceStatic = this->focusDistance;
		// ピントが合う範囲
		focusRangeStatic = this->focusRange;
		// 被写界深度のブラー強度
		dofBlurStrengthStatic = this->dofBlurStrength;
	}
	else
	{
		/*! ライトの設定 */
		// ライトの色
		rc.lightColor = { this->debugLightColor.x, this->debugLightColor.y, this->debugLightColor.z, this->debugLightColor.w };
		// ライトの向き
		rc.lightDirection = { this->debugLightDirection.x, this->debugLightDirection.y, this->debugLightDirection.z, this->debugLightDirection.w };
		// ライトの位置
		rc.lightPosition = this->debugLightPosition;
		// ライトの範囲
		rc.lightRange = this->debugLightRange;

		//! シャドウマップ
		// シャドウマップの位置のオフセット
		shadowMapEyeOffsetStatic = this->debugShadowMapEyeOffset;
		// シャドウマップのバイアス
		shadowMapBiasStatic = this->debugShadowMapBias;

		// 環境光、拡散光、 スペキュラー光の設定
		rc.ambientStrength = this->debugAmbientStrength;  // 環境光
		rc.diffuseStrength = this->debugDiffuseStrength;  // 拡散光
		rc.specularStrength = this->debugSpecularStrength; // スペキュラー光

		/*! フォグの設定 */
		// フォグの色
		rc.fogColor = { this->debugFogColor.x, this->debugFogColor.y, this->debugFogColor.z, 1.0f };
		// フォグの開始
		rc.fogStart = this->debugFogStart;
		// フォグの終了
		rc.fogEnd = this->debugFogEnd;
	}

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
}