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
	std::unique_ptr<Sprite> ganban;
};