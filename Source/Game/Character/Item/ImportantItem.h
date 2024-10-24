#pragma once

#include "Graphics/Shader.h"
#include "Game/Character/Character.h"

// アイテム
class ImportantItem : public Character
{
public:
	ImportantItem() {}
	~ImportantItem() override {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2Dスプライトの描画処理
	virtual void SpriteRender(ID3D11DeviceContext* dc) {};

	// HP表示
	virtual void RenderImportantItemGauge(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection) {};

	// デバッグ用GUI描画
	virtual void DrawDebugGUI() {};

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	// 破棄
	void Destroy();
};