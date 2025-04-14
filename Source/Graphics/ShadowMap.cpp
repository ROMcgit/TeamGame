#include "ShadowMap.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderSetting.h"

#include "Game/Camera/CameraController.h"
#include "Game/Scene/Scene.h"

// 初期化処理
void ShadowMap::Initialize()
{
	// レンダーターゲット
	shadowmap = std::make_unique<RenderTarget>
		(Graphics::Instance().GetDevice(),
			4096, 4096, DXGI_FORMAT_R16_FLOAT, true);

	// コンスタントバッファ生成
	ShaderSetting::CreateConstantBuffer(
		Graphics::Instance().GetDevice(),
		sizeof(CbShadowMap),
		CBShadowMap.GetAddressOf());
}

// 開始処理
void ShadowMap::Begin(const RenderContext& rc)
{
	// slot10をはがす
	ID3D11ShaderResourceView* dummy = nullptr;
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(10, 1, &dummy);

	// レンダーターゲット開始
	shadowmap->Begin();

	// 光の位置からの行列を生成
	DirectX::XMVECTOR Eye    = { rc.lightPosition.x + Scene::shadowMapEyeOffsetStatic.x,
		                         rc.lightPosition.y + Scene::shadowMapEyeOffsetStatic.y, 
		                         rc.lightPosition.z + Scene::shadowMapEyeOffsetStatic.z };

	DirectX::XMVECTOR Target = { rc.lightPosition.x, rc.lightPosition.y, rc.lightPosition.z };
	DirectX::XMMATRIX View   = DirectX::XMMatrixLookAtLH(Eye, Target, { 0,1,0 });
	// 投影変換(平行投影)
	DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(
		Scene::shadowMapWidthStatic,    // 横の範囲
		Scene::shadowMapLengthStatic,   // 縦の範囲
		Scene::shadowMapStartStatic, Scene::shadowMapEndStatic  // 描画する最小・最大距離
	);

	// コンスタントバッファ更新＆セット
	CbShadowMap param;
	DirectX::XMStoreFloat4x4(
		&param.shadowProjection,
		View * Projection);

	// シャドウバイアス
	param.shadowBias = Scene::shadowMapBiasStatic;
	// シャドウパディング
	param.shadowPadding = { 0.0f, 0.0f, 0.0f };

	Graphics::Instance().GetDeviceContext()->UpdateSubresource(CBShadowMap.Get(), 0, 0, &param, 0, 0);

	// Slot10にセット
	ID3D11Buffer* psConstantBuffers[] =
	{
		CBShadowMap.Get(),
	};
	Graphics::Instance().GetDeviceContext()->VSSetConstantBuffers(10, _countof(psConstantBuffers), psConstantBuffers);
	Graphics::Instance().GetDeviceContext()->PSSetConstantBuffers(10, _countof(psConstantBuffers), psConstantBuffers);
}

// 終了処理
void ShadowMap::End()
{
	// レンダーターゲットの終了
	shadowmap->End();

	// ピクセルシェーダーslot10にセット
	shadowmap->SetPSSlot(10);
}