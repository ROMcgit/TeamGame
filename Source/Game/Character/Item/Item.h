#pragma once

#include "Graphics/Shader.h"
#include "Game/Character/Character.h"
#include "Graphics/Graphics.h"

// アイテム
class Item : public Character
{
public:
	Item() {}
	~Item() override {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2Dスプライトの描画処理
	virtual void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) {};

	// デバッグ用GUI描画
	virtual void DrawDebugGUI() {};

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	void Destroy();

	// プレイヤー3との衝突処理
	bool CollisionVsPlayer3();
};