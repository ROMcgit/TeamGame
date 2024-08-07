#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Text.h"
#include "Audio/Audio.h"

// タイトルシーン
class SceneGameOver : public Scene
{
public:
	SceneGameOver() {};
	~SceneGameOver() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Sprite> fadeIn;
	float fadeInView = 1.0f;
	bool sceneOK = false;
	float fadeOutView = 0.0f;
	bool fadeOut = false;

	std::unique_ptr<Text> text[2];
	std::unique_ptr<AudioSource> bgm;
};