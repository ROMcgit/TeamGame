#include "Scene.h"
#include "Game/Camera/Camera.h"

/*! デバッグ用 */
bool Scene::debugControll = false; // デバッグ用に数値を変更できるようにするか
DirectX::XMFLOAT4 Scene::debugLightDirection = { 0.0f, -1.0f, 0.0f, 0.0f }; // ライトの向き
DirectX::XMFLOAT4 Scene::debugLightColor = { 0.8f, 0.8f, 0.8f, 1 };     // ライトの色
DirectX::XMFLOAT3 Scene::debugLightPosition = { 0, 0, 0 };                 // ライトの位置

//! ライトへの影響
float Scene::debugAmbientStrength = 1.3f; // 環境光
float Scene::debugDiffuseStrength = 0.5f; // 拡散光
float Scene::debugSpecularStrength = 0.5f; // スペキュラー光
float Scene::debugLightRange = 1.0f; // ライトの範囲

//! フォグ
DirectX::XMFLOAT3 Scene::debugFogColor = { 0.7f, 0.8f, 0.9f }; // フォグの色
float             Scene::debugFogStart = 80.0f;                // フォグの開始
float             Scene::debugFogEnd = 200.0f;               // フォグの終了

// 描画設定
void Scene::DrawingSettings(Graphics& graphics,
	DirectX::XMFLOAT3 clearColor,
	float ambientStrength,
	float diffuseStrength,
	float specularStrength,
	float lightRange,
	DirectX::XMFLOAT3 fogColor,
	float fogStart, float fogEnd)
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
		rc.lightColor = this->debugLightColor = { this->lightColor.x, this->lightColor.y, this->lightColor.z, this->lightColor.w };
		// ライトの向き
		rc.lightDirection = this->debugLightDirection = { this->lightDirection.x, this->lightDirection.y, this->lightDirection.z, this->lightDirection.w };
		// ライトの位置
		rc.lightPosition = this->debugLightPosition = this->lightPosition;
		// ライトの範囲
		rc.lightRange = this->debugLightRange = lightRange;

		// 環境光、拡散光、 スペキュラー光の設定
		rc.ambientStrength = this->debugAmbientStrength = ambientStrength;  // 環境光
		rc.diffuseStrength = this->debugDiffuseStrength = diffuseStrength;  // 拡散光
		rc.specularStrength = this->debugSpecularStrength = specularStrength; // スペキュラー光

		/*! フォグの設定 */
		// フォグの色
		rc.fogColor = { fogColor.x, fogColor.y, fogColor.z, 1.0f };
		// フォグの色(デバッグ)
		this->debugFogColor = { fogColor.x, fogColor.y, fogColor.z };
		// フォグの開始
		rc.fogStart = this->debugFogStart = fogStart;
		// フォグの終了
		rc.fogEnd = this->debugFogEnd = fogEnd;
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