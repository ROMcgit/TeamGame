#include "Graphics/Graphics.h"
#include "Game/Scene/SceneTitle.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Fade.h"
#include "Audio/BgmManager.h"
#include "Audio/SoundManager.h"

// 初期化
void SceneTitle::Initialize()
{
	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("タイトル", "Data/Audio/Bgm/0.Title.wav");
	bgm.PlayBgm("タイトル", 0.7f);

	SoundManager::Instance().LoadSound("決定", "Data/Audio/Sound/Decision.wav");

	// タイトル画面
	title = std::make_unique<Sprite>("Data/Sprite/Title.png");
	titleText = std::make_unique<Sprite>("Data/Sprite/TitleText.png");

	text = std::make_unique<Sprite>("Data/Sprite/TitleButton.png");

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);
}

// 終了化
void SceneTitle::Finalize()
{
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	fade->Update(elapsedTime);

	// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y |
		GamePad::BTN_START |
		GamePad::BTN_SPACE;
	if (gamePad.GetButtonDown() & anyButton && !setFade && !fade->GetFade())
	{
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			0.5f, 0.5f);

		SoundManager::Instance().PlaySound("決定");

		setFade = true;
	}
	else if (setFade && !fade->GetFade())
	{
		BgmManager::Instance().UnloadBgm("タイトル");

		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}

	if(!fade->GetFade())
		textOpacity += (1.5f * elapsedTime) * (textOpacityUp ? 1 : -1);

	if (textOpacity <= 0.0f || textOpacity >= 1.0f)
	{
		textOpacity = std::clamp(textOpacity, 0.0f, 1.0f);

		textOpacityUp = !textOpacityUp;
	}

	if (setFade)
		textTimer += elapsedTime;
}

// 描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア&レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; //RGBA(0.0〜1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(title->GetTextureWidth());
		float textureHeight = static_cast<float>(title->GetTextureHeight());
		// タイトルスプライト描画
		title->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		textureWidth = static_cast<float>(titleText->GetTextureWidth());
		textureHeight = static_cast<float>(titleText->GetTextureHeight());

		titleText->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		//! 表示時間
		const float viewTimer = 0.1f;

		textureWidth = static_cast<float>(text->GetTextureWidth());
		textureHeight = static_cast<float>(text->GetTextureHeight());

		//! タイトル説明
		if (!setFade)
		{
			text->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, textOpacity);
		}
		else if (fmod(textTimer, viewTimer * 2) < viewTimer)
		{
			text->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}

		fade->Render(dc, graphics);
	}
}