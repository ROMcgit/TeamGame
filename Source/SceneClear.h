#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Text.h"
#include "Audio/Audio.h"

// タイトルシーン
class SceneClear : public Scene
{
public:
	SceneClear() {};
	~SceneClear() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	Sprite* sprite = nullptr;
	std::unique_ptr<Text> text[2];
	std::unique_ptr<AudioSource> bgm;
};