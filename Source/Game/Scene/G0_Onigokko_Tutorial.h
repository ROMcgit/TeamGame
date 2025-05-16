#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// 鬼ごっこチュートリアル
class G0_Onigokko_Tutorial : public Scene
{
public:
	G0_Onigokko_Tutorial() {};
	~G0_Onigokko_Tutorial() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// 画像の更新処理
	void SpriteDirector(float elapsedTime);

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> tutorialSpriteFrame;
	std::unique_ptr<Sprite> tutorialSprite[3];
	DirectX::XMFLOAT2       tutorialSpritePos[3];
	DirectX::XMFLOAT2       startPos[3];
	DirectX::XMFLOAT2       endPos[3];
	int tutorialNum = 0;

	int   directorStep = 0;    // 演出のステップ
	float directorTime = 0.0f; // 演出の時間

	std::unique_ptr<Fade> fade;
	bool setFade = false; // フェードを設定したか

	bool tutorialFinish = false;
};