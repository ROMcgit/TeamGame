#pragma once

#include "Game/Character/Player.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemySika.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Fade.h"
#include "Graphics/Text.h"

// ゲームシーン
//class SceneGame
class SceneGame: public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// 生成処理
	void Newestablishment(float elapsedTime);

	// バナナ生成
	void NewBanana(float elapsedTime);

private:
	std::unique_ptr<Player> player;
	std::unique_ptr<CameraController> cameraController;
	std::unique_ptr<Fade> fade;
	std::unique_ptr<Text> text;
	
	int establishmentCount         = 0;
	float newestablishmentTimer    = 0.0f;
	float newestablishmentMaxTimer = 0.1f;

	bool setFade = false;

	bool newBanana[7]; // バナナを生成したか
	float newBananaWaitTimer = 0.5f; // バナナを出すまでの時間
};