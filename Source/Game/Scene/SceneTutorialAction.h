#pragma once

#include "Game/Character/Player.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemySika.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Fade.h"
#include "Graphics/Text.h"

// タイトルシーン
class SceneTutorialAction : public Scene
{
public:
	SceneTutorialAction() {};
	~SceneTutorialAction() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// 生成処理
	void Newestablishment(float elapsedTime);

private:

	enum class MessageScene
	{
		Message1,
		Message2,
		Message3,
		Message4,
		Message5,
		Message6,
	}messageScene = MessageScene::Message1;

private:
	std::unique_ptr<Text> text;
	DirectX::XMFLOAT2 textPos = { 0, 0 };
	DirectX::XMFLOAT2 textScale = { 13.5f, 13.5f };
	std::unique_ptr<Sprite> message[6];
	int messageNum = 0;
	float messageOpacityWaitTime = 0.0f;
	float messageOpacity = 0.0f;
	int inputCount = false;
	float inputTimer = 0.0f;
	float answerTimer = 0.0f;
	bool messageFinish[5];

	std::unique_ptr<Player> player;
	std::unique_ptr<CameraController> cameraController;
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	float doCameraMovieTimer = 0.0f;

	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Fade> fade;

	bool setFade = false;

	float newItemTimer = 0.0f;
	float newItemMaxTimer = 0.5f;
};