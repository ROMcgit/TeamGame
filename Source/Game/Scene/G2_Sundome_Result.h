#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Text.h"

class Fade;

// 寸止めリザルト
class G2_Sundome_Result : public Scene
{
public:
	G2_Sundome_Result() {};
	~G2_Sundome_Result() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// スコアの演出処理
	void DirectorScore(float elapsedTime);

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> result;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // フェードを設定したか

	std::unique_ptr<Text> text[4];
	DirectX::XMFLOAT2 textPos[4];
	float startTextPosX;
	int textNum = 0;
	float totalScore = 0;

	int   directorStep = 0;
	float directorTime = 0.0f;
	bool  directorFinish = false;
};