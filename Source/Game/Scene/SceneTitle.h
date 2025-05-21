#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {};
	~SceneTitle() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	std::unique_ptr<Sprite> title;
	std::unique_ptr<Sprite> titleText;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // フェードを設定したか

	std::unique_ptr<Sprite> text;
	float textOpacity = 1.0f;
	bool textOpacityUp = false;
	float textTimer = 0.0f;
};