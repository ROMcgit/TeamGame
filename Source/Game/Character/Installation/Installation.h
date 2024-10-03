#pragma once

#include "Graphics/Shader.h"
#include "Game/Character/Character.h"

// 設置物
class Installation : public Character
{
public:
	Installation() {}
	~Installation() override {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2Dスプライトの描画処理
	virtual void SpriteRender(ID3D11DeviceContext* dc) {};

	// HP表示
	virtual void RenderInstallationGauge(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection) {};

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	// 破棄
	void Destroy();
};