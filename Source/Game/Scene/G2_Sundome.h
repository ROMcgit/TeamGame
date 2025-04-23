#pragma once

#include "Game/Character/Player/Player2_Sundome.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"

// ゲームシーン
class G2_Sundome : public Scene
{
public:
	G2_Sundome() {}
	~G2_Sundome() override {}

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

private:
	std::unique_ptr<Fade> fade; // フェード

	std::unique_ptr <Player2_Sundome> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	std::unique_ptr<Sprite> backGround;

	bool movieScene = false; // ムービー中か

	int movieStep   = 0;
	float movieTime = 0.0f;
};