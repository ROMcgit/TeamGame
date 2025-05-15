#pragma once

#include "Game/Character/Player/Player0_Onigokko.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Timer.h"
#include "Graphics/Fade.h"

// ゲームシーン
class G0_Onigokko : public Scene
{
public:
	G0_Onigokko() {}
	~G0_Onigokko() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

	// ムービーシーンを設定
	void SetMovieScene(bool movieScene) { this->movieScene = movieScene; }

	// ムービーシーンかを取得
	bool GetMovieScene() { return movieScene; }

private:
	// プレイヤーの位置制限
	void PlayerPositionControll();

	// カメラのムービー更新処理
	void UpdateCameraMovie(float elapsedTime);

	// 敵生成処理
	void NewEnemy();

public:
	static bool  movieScene; // ムービーシーンか
private:
	DirectX::XMFLOAT3 target = { 0, 0, 0 };

	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Timer>  timer;

	
	float cameraMovieTime = 0.0f; // カメラのムービー時間

	bool movieFade = false;
	float movieWaitTimer = 0.0f;
	bool gameFinishFade = false;

	bool nextOnideathFade = false;

	std::unique_ptr<Fade> fade;
	bool oniDeathFade = false;

	enum class CameraMovieScene
	{
		OniEntry, // 鬼登場
		OniMove,  // 鬼移動
		OniDeath, // 鬼死亡
	}cameraMovieScene;
};