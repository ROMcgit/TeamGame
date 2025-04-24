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

public:
	static bool isEnemy[4];

private:
	CollisionAttackManager collisionAttackManager;

	std::unique_ptr<Fade> fade;

	std::unique_ptr <Player4_OssanTataki> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	std::unique_ptr<Sprite> backGround;

	bool movieScene = false; // ムービー中か

	float movieTime = 0.0f; // ムービー時間
	int   movieStep = 0;    // ムービーステップ

	float newEnemyWaitTime[4]; // 敵の生成の待ち時間
};