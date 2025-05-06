#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// 鬼ごっこゲームオーバー
class G0_Onigokko_GameOver : public Scene
{
public:
	G0_Onigokko_GameOver() {};
	~G0_Onigokko_GameOver() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> gameOver;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // フェードを設定したか
};