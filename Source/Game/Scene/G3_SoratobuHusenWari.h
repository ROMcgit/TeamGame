#pragma once

#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"

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
	void NewBalloon();

public:
	static bool movieScene;

private:
	std::unique_ptr<Fade> fade;

	std::unique_ptr <Player3_SoratobuHusenWari> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体
	std::unique_ptr<Sprite> backGround;
};