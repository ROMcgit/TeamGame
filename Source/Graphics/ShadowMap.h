#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "RenderTarget.h"

class ShadowMap
{
private:
	struct CbShadowMap
	{
		DirectX::XMFLOAT4X4 shadowProjection; // シャドウ
		float  shadowBias;               // シャドウバイアス
		DirectX::XMFLOAT3 shadowPadding; // シャドウのパディング
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> CBShadowMap;

public:
	// 初期化処理
	void Initialize();

	// 開始処理
	void Begin(const RenderContext& rc);

	// 終了処理
	void End();

	// シェーダーリソースビューを取得
	ID3D11ShaderResourceView* GetSRV() { return shadowmap->GetSRV(); }

private:
	std::unique_ptr<RenderTarget> shadowmap;
};
