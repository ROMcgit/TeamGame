#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Fade.h"

// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {};
	~SceneTitle() override {};

	static SceneTitle& Instance()
	{
		static SceneTitle instance;
		return instance;
	}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

public:
	bool gameClear = false;

private:
	std::unique_ptr<Sprite> title;
	std::unique_ptr<Sprite> button;
	bool buttonOpacityDown = false;
	float buttonOpacity = 0.0f;
	float opacitySpeed = 1.0;

	std::unique_ptr<Fade> fade;

	bool setFade = false;
};