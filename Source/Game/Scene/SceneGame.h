#pragma once

#include "Game/Character/Player.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemySlime.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

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
	//Stage* stage = nullptr;
	std::unique_ptr <Player> player;
	std::unique_ptr <CameraController> cameraController;
	std::unique_ptr<Sprite> enemyHp;
};