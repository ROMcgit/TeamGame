#include "SceneGameOver.h"
#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"

// 初期化
void SceneGameOver::Initialize()
{
	// スコア
	text[0] = std::make_unique<Text>();

	fadeIn = std::make_unique<Sprite>("Data/Sprite/画面フェードイン.png");

	sprite = std::make_unique<Sprite>("Data/Sprite/ゲームオーバー.png");

	// 音楽
	Audio& audioManager = Audio::Instance();

	bgm = audioManager.LoadAudioSource("Data/Audio/GameMusic/GameOver.wav");
}

// 終了化
void SceneGameOver::Finalize()
{
}

// 更新処理
void SceneGameOver::Update(float elapsedTime)
{
	// 音楽
	if (bgm)
	{
		bgm->Play(true, 1.0f);
	}

	fadeInView -= 0.005f;

	if (fadeInView < 0)
	{
		fadeInView = 0;
		sceneOK = true;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_START;
	if (gamePad.GetButtonDown() & anyButton && sceneOK == true)
	{
		bgm->Stop();

		SceneLoading* loadingScene = new SceneLoading(new SceneTitle);

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(loadingScene);
	}
}

// 描画処理
void SceneGameOver::Render()
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
		970, 360,
		16, 16,
		0,
		60,
		1, 0, 0, 1);

	// フェードアウト
	fadeIn->Render(dc,
		0, 0,
		2230, 1750,
		0, 0,
		2230, 1750,
		0,
		1, 0, 0, fadeInView);
}