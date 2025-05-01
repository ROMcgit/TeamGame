#pragma once

#include "Game/Character/Player/Player1_DarumasangaKoronda.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"
#include "Graphics/Timer.h"

// ゲームシーン
class G1_DarumasangaKoronda : public Scene
{
public:
	G1_DarumasangaKoronda() {}
	~G1_DarumasangaKoronda() override {}

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

public:
	static bool movieScene; // ムービー中か

private:
	std::unique_ptr<Fade> fade;

	std::unique_ptr <Player1_DarumasangaKoronda> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	std::unique_ptr<Sprite> backGround;

	float movieTime = 0.0f; // ムービー時間
	int   movieStep = 0;    // ムービーステップ

	std::unique_ptr<Timer> timer;
};