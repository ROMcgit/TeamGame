#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneTutorialSelect.h"
#include "SceneTutorial.h"
#include "SceneGameOver.h"
#include "SceneGameClear.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneOpning.h"
#include "SceneManager.h"
#include "Audio/SoundEffectManager.h"

// 初期化
void SceneTutorialSelect::Initialize()
{
	message = std::make_unique<Sprite>("Data/Sprite/チュートリアル/初めて？.png");
	yes = std::make_unique<Sprite>("Data/Sprite/チュートリアル/はい.png");
	no = std::make_unique<Sprite>("Data/Sprite/チュートリアル/いいえ.png");
	banana = std::make_unique<Sprite>("Data/Sprite/UI/バナナ.png");
	decide = std::make_unique<Sprite>("Data/Sprite/背景/決定.png");

	SoundEffectManager::Instance().LoadSoundEffect("決定", "Data/Audio/決定ボタン.wav");
}

// 終了化
void SceneTutorialSelect::Finalize()
{
}

// 更新処理
void SceneTutorialSelect::Update(float elapsedTime)
{
	float textureWidth = static_cast<float>(message->GetTextureWidth());
	float textureHeight = static_cast<float>(message->GetTextureHeight());

	//! メッセージを表示
	if (textureWidth > messageTexW)
		messageTexW += 800 * elapsedTime;
	else if (viewSelectTimer < 1.0f)
		viewSelectTimer += elapsedTime;
	else
		viewSelect = true;


	if (viewSelect)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();

		if (gamePad.GetButtonDown() & GamePad::BTN_UP)
			select--;
		if (gamePad.GetButtonDown() & GamePad::BTN_DOWN)
			select++;
		if (select < 1)
			select = 1;
		if (select > 2)
			select = 2;

		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			SoundEffectManager::Instance().PlaySoundEffect("決定");

			std::unique_ptr<SceneLoading> loadingScene;

			switch (select)
			{
			case 1:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTutorial>());
				// シーンマネージャーにローディングシーンへの切り替えを指示
				SceneManager::Instance().ChangeScene(std::move(loadingScene));
				break;
			case 2:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGame>());
				// シーンマネージャーにローディングシーンへの切り替えを指示
				SceneManager::Instance().ChangeScene(std::move(loadingScene));

				break;
			default:
				break;
			}
		}
	}
}

// 描画処理
void SceneTutorialSelect::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア&レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		float textureWidth = static_cast<float>(message->GetTextureWidth());
		float textureHeight = static_cast<float>(message->GetTextureHeight());
		// メッセージ
		message->Render(dc,
			80, 100, 1120, 100,
			0, 0, messageTexW, textureHeight,
			0,
			1, 1, 1, 1);

		if (viewSelect)
		{
			textureWidth = static_cast<float>(yes->GetTextureWidth());
			textureHeight = static_cast<float>(yes->GetTextureHeight());

			// はい
			yes->Render(dc,
				570, 290, 150, 80,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(no->GetTextureWidth());
			textureHeight = static_cast<float>(no->GetTextureHeight());

			// いいえ
			no->Render(dc,
				550, 480, 200, 80,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);


			textureWidth = static_cast<float>(decide->GetTextureWidth());
			textureHeight = static_cast<float>(decide->GetTextureHeight());

			// 決定
			decide->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(banana->GetTextureWidth());
			textureHeight = static_cast<float>(banana->GetTextureHeight());

			switch (select)
			{
			case 1:
				banana->Render(dc,
					480, 295,
					60, 48,
					0, 0,
					textureWidth, textureHeight,
					0,
					1, 1, 1, 1);
				break;
			case 2:
				banana->Render(dc,
					480, 500,
					60, 48,
					0, 0,
					textureWidth, textureHeight,
					0,
					1, 1, 1, 1);
				break;
			default:
				break;
			}
		}
	}
}