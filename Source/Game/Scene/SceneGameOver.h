#pragma once

#include "Scene.h"
#include "Graphics/Sprite.h"

// ゲームシーン
class SceneGameOver : public Scene
{
public:
	SceneGameOver() {}
	~SceneGameOver() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	std::unique_ptr<Sprite> saru;
	DirectX::XMFLOAT2 saruPos   = { 300, 300 };
	DirectX::XMFLOAT2 saruScale = { 0, 0 };
	std::unique_ptr<Sprite> ganban;
	std::unique_ptr<Sprite> ganbanHekomi;
	DirectX::XMFLOAT2 hekomiPos = { 620, 300 };
	DirectX::XMFLOAT2 hekomiScale = { 0, 0 };
};