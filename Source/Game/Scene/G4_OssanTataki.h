#pragma once

#include "Game/Character/Player/Player4_OssanTataki.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Game/Character/CollisionAttack/CollisionAttackManager.h"
#include "Graphics/Fade.h"
#include "Graphics/Text.h"
#include "Graphics/Timer.h"
#include "Graphics/Pause.h"

// ゲームシーン
class G4_OssanTataki : public Scene
{
public:
	G4_OssanTataki() {}
	~G4_OssanTataki() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// ムービー更新処理
	void UpdateMovie(float elapsedTime);

	// 敵生成処理
	void NewEnemy(float elapsedTime);

	// スコア更新処理
	void UpdateScore();

	// シーン切り替え処理
	void SceneChange();

public:
	static int score;

private:
	std::unique_ptr<Text> scoreText;
	DirectX::XMFLOAT2 scoreTextPos = { 1240.0f, 0.0f };

	std::unique_ptr<Pause> pause;

	CollisionAttackManager collisionAttackManager;

	std::unique_ptr<Fade> fade;
	bool setFade = false;

	std::unique_ptr <Player4_OssanTataki> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	std::unique_ptr<Sprite> backGround;
	DirectX::XMFLOAT3 backGroundColor = { 0.6f, 1, 0.77f };

	bool movieScene = false; // ムービー中か

	float movieTime = 0.0f; // ムービー時間
	int   movieStep = 0;    // ムービーステップ

	bool newEnemy = false;

	std::unique_ptr<Timer> timer;
};