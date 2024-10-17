#pragma once

#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Fade.h"

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
	std::unique_ptr<Fade> fade;
	bool setFade = false;
	std::unique_ptr<Sprite> saru;
	DirectX::XMFLOAT2 saruPos   = { 500, 500 };
	DirectX::XMFLOAT2 saruScale = { 628, 672 };
	float saruAngle = DirectX::XMConvertToRadians(0);

	std::unique_ptr<Sprite> ganban;
	std::unique_ptr<Sprite> ganbanHekomi;
	float hekomiTimer = 0.0f;
	DirectX::XMFLOAT2 hekomiPos = { 620, 300 };
	DirectX::XMFLOAT2 hekomiScale = { 0, 0 };

	std::unique_ptr<Sprite> gameOver;
	float viewGameOverWaitTime = 0.0f;
	float gameOverOpacity = 0.0f;

	bool operationAccept = false;

	bool soundPlay = false;

	std::unique_ptr<Sprite> button;
	float buttonOpacity = 0.0f;
	float buttonOpacitySpeed = 0.5f;
	bool buttonOpacityDown = false;

	enum class SelectScene
	{
		Continue,
		Title,
	}selectScene = SelectScene::Continue;
};