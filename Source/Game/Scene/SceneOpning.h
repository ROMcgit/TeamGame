#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Fade.h"

// タイトルシーン
class SceneOpning : public Scene
{
public:
	SceneOpning() {};
	~SceneOpning() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	enum class SpriteScene
	{
		Home,
		Tv,
		SaruKimaru,
	};

private:

	std::unique_ptr<Fade> fade;
	bool setFade = false;
	std::unique_ptr<Sprite> homeSprite[2];
	std::unique_ptr<Sprite> tvSprite[4];
	std::unique_ptr<Sprite> saruKimaruSprite[2];

	SpriteScene spriteScene = SpriteScene::Home;
	int viewSpriteNum = 0;
	float spriteChangeTimer = 0.0f;
	float sceneChangeTimer  = 0.0f;
	float tvChangeTimer     = 0.0f;
	bool  tvChangeSet       = false;
};