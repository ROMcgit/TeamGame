#pragma once

#include "Player.h"
#include "CameraController.h"
#include "EnemySphere.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include <memory>
#include "Graphics/Text.h"

// ゲームシーン
//class SceneGame
class SceneGame: public Scene
{
public:
	SceneGame() {}
	//~SceneGame() {}
	~SceneGame() override {}

	// 初期化
	//void Initialize();
	void Initialize() override;

	// 終了化
	//void Finalize();
	void Finalize() override;

	// 更新処理
	//void Update(float elapsedTime);
	void Update(float elapsedTime) override;

	// 描画処理
	//void Render();
	void Render() override;

private:

	// プレイヤーUI
	void PlayerUI();

	// エネミーHPゲージ描画
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);

private:
	//Stage* stage = nullptr;
	std::unique_ptr<Player> player;
	std::unique_ptr<Sprite> uiSprite[4];
	CameraController* cameraController = nullptr;
	Sprite* gauge = nullptr;
	std::unique_ptr<Text> text[2];
	float newEnemyCount = 0;
	float newEnemyMaxCount = 10;
	int newWallCount = 0;
	int battleWave = 1;
	int nextWaveWait = 0;
	bool newEnemy = false;
	bool battleStart = false;
	int nextSceneCount = 0;
};