#pragma once

#include "Game/Character/Player.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemySika.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Fade.h"
#include "Graphics/Text.h"

// タイトルシーン
class SceneTutorialAction : public Scene
{
public:
	SceneTutorialAction() {};
	~SceneTutorialAction() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	std::unique_ptr<Sprite> image[11];
	std::unique_ptr<Player> player;
	std::unique_ptr<CameraController> cameraController;
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	float doCameraMovieTimer = 0.0f;

	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Fade> fade;
	std::unique_ptr<Fade> fade;

	bool setFade = false;
};