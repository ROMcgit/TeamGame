#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Fade.h"

// タイトルシーン
class SceneTutorialSelect : public Scene
{
public:
	SceneTutorialSelect() {};
	~SceneTutorialSelect() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	std::unique_ptr<Sprite> message;
	float messageTexW = 0.0f;
	int select = 1;
	std::unique_ptr<Sprite> yes;
	std::unique_ptr<Sprite> no;
	float viewSelectTimer = 0.0f;
	bool viewSelect = false;
	std::unique_ptr<Sprite> banana;

	std::unique_ptr<Fade> fade;

	bool setFade = false;
};