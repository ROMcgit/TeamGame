#pragma once

#include "Game/Character/Player/Player0_Onigokko.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
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

public:
	enum class GameSelect
	{
		Onigokko,           // おにごっこ
		DarumasangaKoronda, // だるまさんが転んだ
		Sundome,            // 寸止め
		SoratobuHusenWari,  // 空飛ぶ風船割り
		OssanTataki,        // おっさん叩き
		Asibawatari,        // 足場渡り
	};

	static GameSelect gameSelect; // ゲーム選択

	static bool sceneChange; // シーン切り替え

private:
	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>    renderTarget; //! レンダーターゲット
	std::unique_ptr<Sprite> backGround;


	std::unique_ptr<Fade> fade;
	bool setFade = false;
};