#pragma once

#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"
#include "Game/Character/CollisionAttack/CollisionAttackManager.h"
#include "Graphics/Text.h"

// ゲームシーン
class G3_SoratobuHusenWari : public Scene
{
public:
	G3_SoratobuHusenWari() {}
	~G3_SoratobuHusenWari() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// 風船生成処理
	void NewBalloon(float elapsedTime);

	// 雲生成処理
	void NewCloud(float elapsedTime);

	// スコア更新処理
	void UpdateScore();

	// シーン変更処理
	void SceneChange();

public:
	static bool movieScene;
	static int score;

private:
	CollisionAttackManager collisionAttackManager;

	std::unique_ptr<Text> scoreText;
	DirectX::XMFLOAT2 scoreTextPos = { 1240.0f, 0.0f };

	std::unique_ptr<Fade> fade;
	bool setFade = false;

	std::unique_ptr <Player3_SoratobuHusenWari> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	std::unique_ptr<Sprite> backGround;

	float newBalloonWaitTime = 0.0f;
	float newCloudWaitTime = 0.0f;
	float gameTimer = 0.0f;
};