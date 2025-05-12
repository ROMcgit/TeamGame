#pragma once

#include "Game/Character/Player/Player0_Onigokko.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"

// ゲームシーン
class SceneGameSelect: public Scene
{
public:
	SceneGameSelect() {}
	~SceneGameSelect() override {}

	// インスタンス取得
	static SceneGameSelect& Instance()
	{
		static SceneGameSelect instance;
		return instance;
	}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	//! プレイヤーの位置制限
	void PlayerPositionControll();

	// ゲーム画面描画
	void RenderGameSprite(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

public:
	enum class GameSelectA
	{
		Onigokko,           // おにごっこ
		DarumasangaKoronda, // だるまさんが転んだ
		Sundome,            // 寸止め
		SoratobuHusenWari,  // 空飛ぶ風船割り
		OssanTataki,        // おっさん叩き
		Asibawatari,        // 足場渡り
	};

	static GameSelectA gameSelect; // ゲーム選択

	static bool sceneChange; // シーン切り替え

private:
	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体

	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> gameSelectSprite[6];
	DirectX::XMFLOAT2 gameSelectSpritePos;

	std::unique_ptr<Fade> fade;
	bool setFade = false;
};