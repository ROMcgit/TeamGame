#include "SceneGameOver.h"
#include "SceneGame.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneTitle.h"
#include "Input/Input.h"
#include "Audio/SoundEffectManager.h"

// 初期化
void SceneGameOver::Initialize()
{
	saru         = std::make_unique<Sprite>("Data/Sprite/背景/Saru3.png");
	ganban       = std::make_unique<Sprite>("Data/Sprite/背景/岩盤.png");
	ganbanHekomi = std::make_unique<Sprite>("Data/Sprite/背景/岩盤凹み.png");
	gameOver     = std::make_unique<Sprite>("Data/Sprite/背景/ゲームオーバー.png");
	button       = std::make_unique<Sprite>("Data/Sprite/コンティニューするか.png");

	//! フェード
	fade = std::make_unique<Fade>();

	SoundEffectManager& sound = SoundEffectManager::Instance();
	sound.LoadSoundEffect("飛行機", "Data/Audio/飛行機.wav");
	sound.PlaySoundEffect("飛行機");
	sound.LoadSoundEffect("岩盤", "Data/Audio/岩盤.wav");
	sound.LoadSoundEffect("決定", "Data/Audio/決定ボタン.wav");
}

// 終了化
void SceneGameOver::Finalize()
{
}

// 更新処理
void SceneGameOver::Update(float elapsedTime)
{
	if (saruAngle < DirectX::XMConvertToRadians(180))
		saruAngle -= DirectX::XMConvertToRadians(10000) * elapsedTime;

	if (saruPos.x < 620)
		saruPos.x += 500 * elapsedTime;

	if (saruPos.y > 310)
		saruPos.y -= 200 * elapsedTime;

	// サルの大きさX
	if (saruScale.x > 0)
		saruScale.x -= 600 * elapsedTime;

	// 大きさを0にする
	if (saruScale.x < 0)
		saruScale.x = 0;

	// サルの大きさY
	if (saruScale.y > 0)
		saruScale.y -= 600 * elapsedTime;

	// 大きさを0にする
	if (saruScale.y < 0)
		saruScale.y = 0;

	if(hekomiTimer > 1.05f)
	{
		if (!soundPlay)
		{
			SoundEffectManager::Instance().PlaySoundEffect("岩盤");
			soundPlay = true;
		}

		if (hekomiScale.x < 1280.0f)
			hekomiScale.x += 1600 * elapsedTime;
		else if(viewGameOverWaitTime < 0.5f)
			viewGameOverWaitTime += elapsedTime;

		if (hekomiScale.y < 720.0f)
			hekomiScale.y += 900 * elapsedTime;

		if (hekomiPos.x > 0)
			hekomiPos.x -= 775 * elapsedTime;

		if (hekomiPos.y > 0)
			hekomiPos.y -= 370 * elapsedTime;
	}
	hekomiTimer += elapsedTime;

	if (viewGameOverWaitTime >= 0.5f && gameOverOpacity < 1.0f)
	{
		viewGameOverWaitTime = 1.0f;
		gameOverOpacity += 2 * elapsedTime;
	}
	else if (viewGameOverWaitTime >= 1.0f)
		operationAccept = true;

	if (gameOverOpacity >= 1.0f)
	{
		if (!buttonOpacityDown)
		{
			buttonOpacity += buttonOpacitySpeed * elapsedTime;
			if (buttonOpacity >= 1.0f)
			{
				buttonOpacity = 1.0f;
				buttonOpacityDown = true;
			}
		}
		else
		{
			buttonOpacity -= buttonOpacitySpeed * elapsedTime;
			if (buttonOpacity <= 0.0f)
			{
				buttonOpacity = 0.0f;
				buttonOpacityDown = false;
			}
		}
	}

	//! 操作を受け付けているなら
	if (operationAccept)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();

		if (!setFade)
		{
			if (gamePad.GetButtonDown() & GamePad::BTN_A)
			{
				selectScene = SelectScene::Continue;

				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					3.0f);

				SoundEffectManager::Instance().PlaySoundEffect("決定", 0.7f);

				setFade = true;
			}
			else if (gamePad.GetButtonDown() & GamePad::BTN_B)
			{
				selectScene = SelectScene::Title;

				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					3.0f);

				SoundEffectManager::Instance().PlaySoundEffect("決定", 0.7f);

				setFade = true;
			}
		}

		if (setFade && !fade->Update(elapsedTime))
		{
			std::unique_ptr<SceneLoading> loadingScene;

			switch (selectScene)
			{
			case SelectScene::Continue:
			{
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGame>());
			}
				break;
			case SelectScene::Title:
			{
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());
			}
				break;
			default:
				break;
			}
			
			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}

// 描画処理
void SceneGameOver::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	float screenWidth  = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	float textureWidth  = static_cast<float>(ganban->GetTextureWidth());
	float textureHeight = static_cast<float>(ganban->GetTextureHeight());

	ganban->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	ganbanHekomi->Render(dc,
		hekomiPos.x, hekomiPos.y,
		hekomiScale.x, hekomiScale.y,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	textureWidth  = static_cast<float>(saru->GetTextureWidth());
	textureHeight = static_cast<float>(saru->GetTextureHeight());

	saru->Render(dc,
		saruPos.x, saruPos.y,
		saruScale.x, saruScale.y,
		0, 0,
		textureWidth, textureHeight,
		saruAngle,
		1, 1, 1, 1);

	textureWidth  = static_cast<float>(gameOver->GetTextureWidth());
	textureHeight = static_cast<float>(gameOver->GetTextureHeight());

	gameOver->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, gameOverOpacity);

	textureWidth = static_cast<float>(button->GetTextureWidth());
	textureHeight = static_cast<float>(button->GetTextureHeight());

	button->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, buttonOpacity);

	fade->Render(dc);
}
