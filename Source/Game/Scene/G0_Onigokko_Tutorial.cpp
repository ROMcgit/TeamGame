#include "Graphics/Graphics.h"
#include "Game/Scene/G0_Onigokko_Tutorial.h"
#include "Game/Scene/G0_Onigokko.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Fade.h"
#include "Other/Easing.h"

// 初期化
void G0_Onigokko_Tutorial::Initialize()
{
	backGround = std::make_unique<Sprite>("Data/Sprite/GameSelect/0.png");

	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// チュートリアル画像
	float posX = screenWidth * 1.3f;
	for(int i = 0; i < 3; i++)
	{
		std::string filePath = "Data/Sprite/0.Onigokko/Tutorial" + std::to_string(i) + ".png";

		tutorialSprite[i] = std::make_unique<Sprite>(filePath.c_str());
		tutorialSpritePos[i].x = posX;
		posX += screenWidth;
		
		tutorialSpritePos[i].y = screenHeight * 0.5f;
	}

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);
}

// 終了化
void G0_Onigokko_Tutorial::Finalize()
{
}

// 更新処理
void G0_Onigokko_Tutorial::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//! フェードの更新処理
	fade->Update(elapsedTime);

	// 画像の演出
	SpriteDirector(elapsedTime);

	//! チュートリアルを終わるなら
	if (tutorialFinish && !fade->GetFade())
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G0_Onigokko>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// 描画処理
void G0_Onigokko_Tutorial::Render()
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
		
		float textureWidth = static_cast<float>(backGround->GetTextureWidth());
		float textureHeight = static_cast<float>(backGround->GetTextureHeight());

		// 背景
		backGround->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			0.5f, 0.5f, 0.5f, 1.0f);

		for (int i = 0; i < 3; i++)
		{
			textureWidth = static_cast<float>(tutorialSprite[i]->GetTextureWidth());
			textureHeight = static_cast<float>(tutorialSprite[i]->GetTextureHeight());
			// チュートリアル画像
			tutorialSprite[i]->RenderCenter(dc,
				tutorialSpritePos[i].x, tutorialSpritePos[i].y,
				screenWidth * 0.5f, screenHeight * 0.5f,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}

		fade->Render(dc, graphics);
	}
}

// 画像の更新処理
void G0_Onigokko_Tutorial::SpriteDirector(float elapsedTime)
{
	if (tutorialFinish) return;

	GamePad& gamePad = Input::Instance().GetGamePad();
	GamePadButton button = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	if (gamePad.GetButtonDown() & GamePad::BTN_START)
	{
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			0.5f, 0.2f);

		tutorialFinish = true;
	}

	switch (directorStep)
	{
	//! 待ち時間
	case 0:

		directorTime += elapsedTime;

		if (directorTime > 2.0f)
		{
			directorTime = 0.0f;
			directorStep++;
		}

		break;
	case 1:
	{

		directorTime += elapsedTime;

		float t = directorTime / 1.0f;

		float screenWidth = Graphics::Instance().GetScreenWidth();
		float screenHeight = Graphics::Instance().GetScreenHeight();

		if(t < 1.0f)
		{
			tutorialSpritePos[0].x = Easing::EaseOut(screenWidth * 1.3f, (screenWidth * 0.5f), t);
			tutorialSpritePos[1].x = Easing::EaseOut(screenWidth * 1.3f + screenWidth, screenWidth * 1.3f, t);
			tutorialSpritePos[2].x = Easing::EaseOut((screenWidth * 1.3f) + (screenWidth * 2), screenWidth * 1.3f + screenWidth, t);
		}
		else
		{
			if (gamePad.GetButtonDown() & button)
			{
				directorTime = 0.0f;
				directorStep++;
			}
		}
	}
		break;
	case 2:
	{

		directorTime += elapsedTime;

		float t = directorTime / 1.0f;

		float screenWidth = Graphics::Instance().GetScreenWidth();
		float screenHeight = Graphics::Instance().GetScreenHeight();

		if (t < 1.0f)
		{
			tutorialSpritePos[0].x = Easing::EaseOut(screenWidth * 0.5f, screenWidth * -0.3f, t);
			tutorialSpritePos[1].x = Easing::EaseOut(screenWidth * 1.3f, screenWidth * 0.5f, t);
			tutorialSpritePos[2].x = Easing::EaseOut(screenWidth * 1.3f + screenWidth, screenWidth * 1.3f, t);
		}
		else
		{
			if (gamePad.GetButtonDown() & button)
			{
				directorTime = 0.0f;
				directorStep++;
			}
		}
	}
		break;
	case 3:
	{

		directorTime += elapsedTime;

		float t = directorTime / 1.0f;

		float screenWidth = Graphics::Instance().GetScreenWidth();
		float screenHeight = Graphics::Instance().GetScreenHeight();

		if (t < 1.0f)
		{
			tutorialSpritePos[1].x = Easing::EaseOut(screenWidth * 0.5f, screenWidth * -0.3f, t);
			tutorialSpritePos[2].x = Easing::EaseOut(screenWidth * 1.3f, screenWidth * 0.5f, t);
		}
		else
		{
			if (gamePad.GetButtonDown() & button)
			{
				directorTime = 0.0f;
				directorStep++;
			}
		}
	}
		break;
	case 4:
	{

		directorTime += elapsedTime;

		float t = directorTime / 1.0f;

		float screenWidth = Graphics::Instance().GetScreenWidth();
		float screenHeight = Graphics::Instance().GetScreenHeight();

		// チュートリアル画像
		float posX = screenWidth * 1.3f;

		if (t < 1.0f)
		{
			tutorialSpritePos[2].x = Easing::EaseOut(screenWidth * 0.5f, screenWidth * -0.3f, t);
		}
		else
		{
			directorTime = 0.0f;
			directorStep++;

		}
	}
		break;
	case 5:

		directorTime += elapsedTime;

		if (directorTime > 0.5f)
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				0.5f, 0.2f);

			tutorialFinish = true;
		}

		break;
	default:
		break;
	}
}