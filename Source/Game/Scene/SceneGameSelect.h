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

	// ボーナス画像の表示処理
	void UpdateBonusImage(float elapsedTime);


public:
#if 1
	enum class GameSelectA
	{
		GameSelect,         // ゲーム選択
		Onigokko,           // おにごっこ
		DarumasangaKoronda, // だるまさんが転んだ
		Sundome,            // 寸止め
		SoratobuHusenWari,  // 空飛ぶ風船割り
		OssanTataki,        // おっさん叩き
		Asibawatari,        // 足場渡り
	};

	static GameSelectA gameSelect; // ゲーム選択

	static bool sceneChange; // シーン切り替え

	//! クリア
	struct Clear
	{
		bool onigokko           = false; // おにごっこ
		bool darumasangaKoronda = false; // だるまさんが転んだ
		bool sundome            = false; // 寸止め
		bool soratobuHusenWari  = false; // 空飛ぶ風船割り
		bool ossanTataki        = false; // おっさん叩き
		bool asibawatari        = false; // 足場渡り
	};

	static Clear clear;

#endif

private:
	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<Sprite> gameExplanation; // ゲームの説明
	float gameExplanationOpacity = 1.0f;     // ゲームの説明の不透明度
	bool gameExplanationOpacityUp = false;   // ゲームの説明に不透明度を上昇させるか

	std::unique_ptr<Sprite> bonusExplanation; // ボーナスの説明
	std::unique_ptr<Sprite> bonusUnlock;      // ボーナスの解放条件
	std::unique_ptr<Sprite> bonusL;           // 選択の左矢印
	std::unique_ptr<Sprite> bonusR;           // 選択の右矢印

	std::unique_ptr<RenderTarget>  renderTarget; //! レンダーターゲット
	ShadowMap                      shadowMap;    // シャドウマップの実体

	std::unique_ptr<Sprite> backGround;          // 背景
	std::unique_ptr<Sprite> gameSelectSprite[6]; // ゲーム選択
	DirectX::XMFLOAT2 gameSelectSpritePos;       // ゲーム選択の位置

	bool viewBonusImage = false;                // ボーナス画像を表示するか
	std::unique_ptr<Sprite> hint[7];            // ボーナスのヒント
	std::unique_ptr<Sprite> bonusImage[7];      // ボーナス画像
	std::unique_ptr<Sprite> bonusImageFrame[7]; // ボーナス画像の枠
	DirectX::XMFLOAT3 bonusImageFrameColor = { 0, 0.69f, 1.0f }; // ボーナス画像の枠の色
	bool gameComplete = false; // ゲームをコンプリートしたか

	int   bonusImageNum = 1;        // ボーナス画像の番号
	float bonusImageOpacity = 0.0f; // ボーナス画像の不透明度
	float bonusImagePosX[7];        // ボーナス画像の位置
	float startBonusImagePosX[7];   // ボーナス画像の移動開始位置
	float endBonusImagePosX[7];     // ボーナス画像の移動終了位置
	float bonusImageColor[7];       // ボーナス画像色
	bool bonusImageMove = false;    // ボーナス画像の移動させるか
	float bonusImageElapsedTime = 0.0f; // ボーナス画像の移動の経過時間
	float inputWaitTime = 0.0f;         // 入力待ち時間

	std::unique_ptr<Fade> fade; // フェード
	bool setFade = false;       // フェードを設定したか
};