#pragma once

#include "Game/Character/Player/Player0_Onigokko.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemySlime.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"

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

private:
	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>    renderTarget; //! レンダーターゲット
	std::unique_ptr<Sprite> backGround;
};