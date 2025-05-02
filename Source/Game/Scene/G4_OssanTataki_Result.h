#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Text.h"

class Fade;

// 空飛ぶ風船割りリザルト
class G4_OssanTataki_Result : public Scene
{
public:
	G4_OssanTataki_Result() {};
	~G4_OssanTataki_Result() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// スコア画像の更新処理
	void UpdateScoreSprite(float elapsedTime);

	int   scoreStep = 0;
	float scoreTimer = 0.0f;
	bool scoreDirector = false;
	DirectX::XMFLOAT2 scoreScale = { 30, 30 };
	float space = 100.0f;
	bool left = false;

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // フェードを設定したか

	std::unique_ptr<Text> score;
	DirectX::XMFLOAT2 scorePos = { 0, 0 };
	float startScorePosX;
};