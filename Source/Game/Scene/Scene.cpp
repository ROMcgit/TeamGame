#include "Scene.h"
#include "Game/Camera/Camera.h"

DirectX::XMFLOAT4 Scene::lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f }; // ライトの向き
DirectX::XMFLOAT4 Scene::lightColor = { 0.8f, 0.8f, 0.8f, 1 };     // ライトの色
DirectX::XMFLOAT3 Scene::lightPosition = { 0, 0, 0 };                 // ライトの位置

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

	// ライトの設定
	rc.lightColor = { this->lightColor.x, this->lightColor.y, this->lightColor.z, this->lightColor.w };
	rc.lightDirection = { this->lightDirection.x, this->lightDirection.y, this->lightDirection.z, this->lightDirection.w }; // ライト方向（下方向）
	rc.lightPosition = this->lightPosition;
	rc.lightRange = lightRange;

	// 環境光、拡散光、 スペキュラー光の設定
	rc.ambientStrength = ambientStrength;
	rc.diffuseStrength = diffuseStrength;
	rc.specularStrength = specularStrength;

	// フォグの設定
	rc.fogColor = { fogColor.x, fogColor.y, fogColor.z, 1.0f }; // フォグの色
	rc.fogStart = fogStart;                                     // フォグの開始
	rc.fogEnd   = fogEnd;                                       // フォグの終了

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
}