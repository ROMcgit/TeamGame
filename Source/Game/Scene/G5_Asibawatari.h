#pragma once

#include "Game/Character/Player/Player5_AsibaWatari.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"
#include "Graphics/Pause.h"
#include "Game/Sky.h"

// ゲームシーン
class G5_Asibawatari : public Scene
{
public:
	G5_Asibawatari() {}
	~G5_Asibawatari() override {}

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

	// ステージ生成処理
	void NewStage(float elapsedTime);

	// シーン切り替え処理
	void SceneChange();

public:
	static bool gameClear;

private:
	std::unique_ptr<Fade> fade;
	bool setFade = false;

	std::unique_ptr<Pause> pause;

	std::unique_ptr<Sky> sky;

	std::unique_ptr <Player5_AsibaWatari> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	std::unique_ptr<Sprite> backGround;
	DirectX::XMFLOAT3 backGroundColor = { 0, 0, 0 };

	std::unique_ptr<Sprite> actionExplanation;

	bool movieScene = false; // ムービー中か
	float movieTime = 0.0f;  // ムービー時間
	int   movieStep = 0;     // ムービーステップ

	float gameTimer = 0.0f;

	int stageStep = 0;

	bool newStageFinish = false;
};