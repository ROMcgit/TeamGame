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

	// ムービー
	void UpdateMovie(float elapsedTime);

private:
	std::unique_ptr<Player> player;
	std::unique_ptr<CameraController> cameraController;
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	float doCameraMovieTimer = 0.0f;

	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Fade> fade;
	
	int enemyMaxCount      = 0;
	float newEnemyTimer    = 0.0f;
	float newEnemyMaxTimer = 0.1f;

	float newItemTimer    = 0.0f;
	float newItemMaxTimer = 0.5f;

	float newInstallationTimer    = 0.0f;
	float newInstallationMaxTimer = 1.0f;


	bool setFade = false;

	bool newBanana[6]; // バナナを生成したか
	float newBananaWaitTimer = 0.5f; // バナナを出すまでの時間

	bool enemyStop     = false;
	bool enemyDelete   = false;
	bool setMovie      = false;
	bool setMovieFade  = false;
	bool newSikaTentyo = false;

	DirectX::XMFLOAT3 cameraTarget = { 0, 0, 0 }; // カメラのターゲット
	DirectX::XMFLOAT3 cameraAngle  = { 0, 0, 0 }; // カメラの角度
	float cameraRange = 0.0f;
	bool setCameraShake[2];

	bool setGameClearMovie = false;
};