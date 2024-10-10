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
		SaruRan,
		ShoppingMall,
		SaruKireru,
		UnkoAttack,
		UnkoHit,
		SaruOdoroku,
		SikaRash,
	};

private:

	std::unique_ptr<Fade> fade;
	bool setFade = false;
	std::unique_ptr<Sprite> homeSprite[2];
	std::unique_ptr<Sprite> tvSprite[4];
	std::unique_ptr<Sprite> saruKimaruSprite[2];
	std::unique_ptr<Sprite> house[2];

	std::unique_ptr<Sprite> shopping;
	float shoppingPosY = 700.0f;
	std::unique_ptr<Sprite> saru[3];
	float saruScalePlusTimer = 0.0f;
	DirectX::XMFLOAT2 saruPos = { 830 ,650 };
	DirectX::XMFLOAT2 saruScale = { 0, 0 };

	std::unique_ptr<Sprite> unko;
	float unkoScaleTimer = 0.0f;
	DirectX::XMFLOAT2 unkoPos   = { 600, 400 };
	DirectX::XMFLOAT2 unkoScale = { 0, 0 };
	float unkoAngle = DirectX::XMConvertToRadians(0);

	std::unique_ptr<Sprite> sika[2];
	float sikaMoveTimer = 0.0f;
	float sikaPosX = 1500;

	DirectX::XMFLOAT2 sikaScale = { 724, 510 };
	std::unique_ptr<Sprite> sikaFinal[100];
	DirectX::XMFLOAT2 sikaPos[100];

	SpriteScene spriteScene = SpriteScene::Home;
	int viewSpriteNum = 0;
	float spriteChangeTimer = 0.0f;
	float sceneChangeTimer  = 0.0f;
	float tvChangeTimer     = 0.0f;
	bool  tvChangeSet       = false;
};