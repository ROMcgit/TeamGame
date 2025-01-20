#pragma once

#include "Graphics/Shader.h"
#include "Game/Character/Character.h"
#include "Graphics/Graphics.h"

// エネミー
class Obstacle : public Character
{
public:
	Obstacle() {}
	~Obstacle() override {}

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
};