#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Fade.h"

// タイトルシーン
class SceneGameClear : public Scene
{
public:
	SceneGameClear() {};
	~SceneGameClear() override {};

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
		SikaDown,
		CollapseShoppingMall,
		MonTube1,
		MonTube2,
		House,
		Arrest,
		ThankYou,
	};

private:
	SpriteScene spriteScene = SpriteScene::SikaDown;
	std::unique_ptr<Fade> fade;
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> shopping;
	std::unique_ptr<Sprite> collapseShopping;
	std::unique_ptr<Sprite> sika;
	float sikaPosY = -20;
	float sikaAngle = DirectX::XMConvertToRadians(0);

	std::unique_ptr<Sprite> monTube[2]; // モンチューブ
	bool setMontubefade = false;

	std::unique_ptr<Sprite> house[2]; 
	int viewSpriteNum = 0;

	std::unique_ptr<Sprite> arrestTv;
	bool setArrestFade = false;

	std::unique_ptr<Sprite> thankYou;
	float thankOpacity = 0.0f;

	float sceneChangeTimer  = 0.0f;
	float spriteChangeTimer = 0.0f;

	bool sceneChange = false;
};