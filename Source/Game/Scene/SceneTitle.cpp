#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneTitle.h"
#include "SceneGameOver.h"
#include "SceneTutorialSelect.h"
#include "SceneGameClear.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneOpning.h"
#include "SceneManager.h"
#include "Audio/BgmManager.h"

// 初期化
void SceneTitle::Initialize()
{
	SceneTitle& scene = SceneTitle::Instance();
	scene.gameClear = false;

	BgmManager::Instance().LoadBgm("タイトル", "Data/Audio/bgm/famipop3.wav");
	BgmManager::Instance().PlayBgm("タイトル", 0.5f);

	title = std::make_unique<Sprite>("Data/Sprite/サル系MonTuber.png");
	button = std::make_unique<Sprite>("Data/Sprite/サル系MonTuber(ボタン).png");

	//! フェード
	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		4.0f);
}

// 終了化
void SceneTitle::Finalize()
{
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
	if (!buttonOpacityDown)
	{
		buttonOpacity += opacitySpeed * elapsedTime;
		if (buttonOpacity >= 1.0f)
		{
			buttonOpacity = 1.0f;
			buttonOpacityDown = true;
		}
	}
	else
	{
		buttonOpacity -= opacitySpeed * elapsedTime;
		if (buttonOpacity <= 0.0f)
		{
			buttonOpacity = 0.0f;
			buttonOpacityDown = false;
		}
	}

	if (!fade->GetFade())
	{
		GamePad& gamePad = Input::Instance().GetGamePad();

		// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
		const GamePadButton anyButton =
			GamePad::BTN_A |
			GamePad::BTN_B;
		if (gamePad.GetButtonDown() & anyButton)
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				3.0f);

			setFade = true;
		}

		if (setFade && !fade->GetFade())
		{
			BgmManager::Instance().UnloadBgm("タイトル");

			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneOpning>());

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}

	//! フェード更新処理
	fade->Update(elapsedTime);
}

// 描画処理
void SceneTitle::Render()
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

		// タイトルボタン描画
		button->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, buttonOpacity);
	}

	fade->Render(dc);
}