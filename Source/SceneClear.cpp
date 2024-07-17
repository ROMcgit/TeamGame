#include "SceneClear.h"
#include "Camera.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneGameOver.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include <Graphics\Graphics.h>

// 初期化
void SceneClear::Initialize()
{
	// スコア
	text[0] = std::make_unique<Text>();

	sprite = std::make_unique<Sprite>("Data/Sprite/クリア.png");

	// 音楽
	Audio& audioManager = Audio::Instance();

	bgm = audioManager.LoadAudioSource("Data/Audio/GameMusic/GameClear.wav");
}

// 終了化
void SceneClear::Finalize()
{
}

// 更新処理
void SceneClear::Update(float elapsedTime)
{
	if (bgm)
	{
		bgm->Play(true, 1.0f);
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_START;
	if (gamePad.GetButtonDown() & anyButton)
	{
		bgm->Stop();

		SceneLoading* loadingScene = new SceneLoading(new SceneTitle);

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(loadingScene);
	}
}

// 描画処理
void SceneClear::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア&レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; //RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	sprite->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		1280, 720,
		0,
		1, 1, 1, 1);

	SceneTitle& title = SceneTitle::Instance();

	text[0]->Render(dc,
		true, false,
		false,
		0, 0, 0, 0, 0,
		0, 0, 0, title.score,
		530, 360,
		15, 15,
		0,
		60,
		1, 1, 0, 1);
}