#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// だるまさんが転んだクリア
class G1_DarumasangaKoronda_Clear : public Scene
{
public:
	G1_DarumasangaKoronda_Clear() {};
	~G1_DarumasangaKoronda_Clear() override {};

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
	std::unique_ptr<Sprite> gameClear;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // フェードを設定したか

	float sceneChangeTimer = 0.0f;
};