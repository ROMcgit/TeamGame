#include "Graphics/Graphics.h"
#include "Game/Scene/G1_DarumasangaKoronda_Tutorial.h"
#include "Game/Scene/G1_DarumasangaKoronda.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Fade.h"
#include "Other/Easing.h"
#include "Audio/BgmManager.h"

// 初期化
void G1_DarumasangaKoronda_Tutorial::Initialize()
{
	backGround = std::make_unique<Sprite>("Data/Sprite/GameSelect/1.png");

	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// チュートリアル画像
	float posX = screenWidth * 1.7f;
	for (int i = 0; i < 3; i++)
	{
		std::string filePath = "Data/Sprite/1.DarumasangaKoronda/Tutorial" + std::to_string(i) + ".png";

		tutorialSprite[i] = std::make_unique<Sprite>(filePath.c_str());
		tutorialSpritePos[i].x = posX;
		posX += screenWidth;

		//! 開始の位置
		startPos[i].x = tutorialSpritePos[i].x;
		endPos[i].x = (screenWidth * 0.5f) + (i * screenWidth * 1.7f);

		tutorialSpritePos[i].y = screenHeight * 0.5f;
	}

	tutorialSpriteFrame = std::make_unique<Sprite>();

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);
}

// 終了化
void G1_DarumasangaKoronda_Tutorial::Finalize()
{
}

// 更新処理
void G1_DarumasangaKoronda_Tutorial::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//! フェードの更新処理
	fade->Update(elapsedTime);

	// 画像の演出
	SpriteDirector(elapsedTime);

	//! チュートリアルを終わるなら
	if (tutorialFinish && !fade->GetFade())
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G1_DarumasangaKoronda>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// 描画処理
void G1_DarumasangaKoronda_Tutorial::Render()
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
		
			// チュートリアルの画像の枠
			tutorialSpriteFrame->RenderCenter(dc,
				tutorialSpritePos[i].x, tutorialSpritePos[i].y,
				screenWidth * 0.67f, screenHeight * 0.68f,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			// チュートリアル画像
			tutorialSprite[i]->RenderCenter(dc,
				tutorialSpritePos[i].x, tutorialSpritePos[i].y,
				screenWidth * 0.65f, screenHeight * 0.65f,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}

		fade->Render(dc, graphics);
	}
}

// 画像の更新処理
void G1_DarumasangaKoronda_Tutorial::SpriteDirector(float elapsedTime)
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

		if (t < 1.0f)
		{
			for (int i = 0; i < 3; i++)
				tutorialSpritePos[i].x = Easing::EaseOut(startPos[i].x, endPos[i].x, t);
		}
		else
		{
			if (gamePad.GetButtonDown() & button)
			{
				if (tutorialNum >= 2)
				{
					directorStep++;
				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						startPos[i].x = tutorialSpritePos[i].x;
						endPos[i].x -= screenWidth * 1.7f;
					}

					tutorialNum++;
				}

				directorTime = 0.0f;
			}
		}
	}
	break;
	case 2:

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