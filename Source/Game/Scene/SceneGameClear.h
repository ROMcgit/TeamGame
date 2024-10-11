#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

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
	};

private:
	SpriteScene spriteScene = SpriteScene::SikaDown;
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> shopping;
	std::unique_ptr<Sprite> collapseShopping;
	std::unique_ptr<Sprite> sika;
	float sikaPosY = -20;
	float sikaAngle = DirectX::XMConvertToRadians(0);
};