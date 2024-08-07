#pragma once

#include "Player.h"
#include "CameraController.h"
#include "EnemySphere.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include <memory>
#include "Graphics/Text.h"
#include "Audio/Audio.h"

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
	void PlayerUI(ID3D11DeviceContext* dc);

	// 文字表示
	void DrawFont(ID3D11DeviceContext* dc);

	// エネミーHPゲージ描画
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);

private:
	//Stage* stage = nullptr;
	std::unique_ptr<Player> player;
	std::unique_ptr<Sprite> uiSprite[5];
	std::unique_ptr<Sprite> wave[3];
	std::unique_ptr<Sprite> setumei[2];
	std::unique_ptr<Sprite> bg;
	std::unique_ptr<Sprite> fadeIn;
	float fadeInViewStart = 1.0f;
	float fadeInView = 0.0f;
	CameraController* cameraController = nullptr;
	Sprite* EnemyHp = nullptr;
	std::unique_ptr<Text> text[4];
	std::unique_ptr<AudioSource> bgm;
	DirectX::XMFLOAT3 setumeiColor = { 0,0,0 };
	bool setumeiColorMinus[3];
	DirectX::XMFLOAT4 comboColor = { 0,0,0,0 };
	bool comboColorMinus[4];

	float newEnemyCount = 0;
	int newEnemyLimit = 0;
	float newEnemyMaxCount = 10;
	int newWallCount = 0;
	int battleWave = 0;
	int nextWaveWait = 0;
	float waveFadeIn = 0;
	bool newEnemy = false;
	bool battleStart = false;
	int nextSceneCount = 0;
};