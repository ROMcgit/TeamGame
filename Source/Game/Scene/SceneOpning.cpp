#include "SceneOpning.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneGame.h"
#include "SceneManager.h"

// 初期化
void SceneOpning::Initialize()
{
	//! 家の中の画像
	for (int i = 0; i < 2; i++)
	{
		//! 家の中(テレビを見ている)
		std::string filePath = "Data/Sprite/背景/家の中" + std::to_string(i + 1) + ".png";
		homeSprite[i] = std::make_unique<Sprite>(filePath.c_str());

		//! サルがキマル
		filePath = "Data/Sprite/背景/サルがキマル" + std::to_string(i + 1) + ".png";
		saruKimaruSprite[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	//! テレビの映像画像
	for (int i = 0; i < 4; i++)
	{
		std::string filePath = "Data/Sprite/背景/テレビ" + std::to_string(i + 1) + ".png";
		tvSprite[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	//! フェード
	fade = std::make_unique<Fade>();
}

// 終了化
void SceneOpning::Finalize()
{
}

// 更新処理
void SceneOpning::Update(float elapsedTime)
{


	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_A && !setFade)
	{
		setFade = true;

		// フェードをセット
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			3.0f);
	}

	//! フェードが終わったらシーン遷移
	if (setFade && !fade->Update(elapsedTime))
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGame>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}

	switch (spriteScene)
	{
	case SceneOpning::SpriteScene::Home:
	{
		spriteChangeTimer += elapsedTime;
		sceneChangeTimer += elapsedTime;

		//! 画像シーンを切り替える
		if (sceneChangeTimer > 3.0f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::Tv;
		}

		if (spriteChangeTimer > 0.1f && viewSpriteNum != 1)
		{
			spriteChangeTimer = 0.0f;
			viewSpriteNum++;
		}
		else if (spriteChangeTimer > 0.1f)
		{
			spriteChangeTimer = 0.0f;
			viewSpriteNum--;
		}
	}
		break;
	case SceneOpning::SpriteScene::Tv:
	{
		spriteChangeTimer += elapsedTime;
		tvChangeTimer     += elapsedTime;
		sceneChangeTimer += elapsedTime;

		//! 画像シーンを切り替える
		if (sceneChangeTimer > 6.5f)
		{
			viewSpriteNum = 0;
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::SaruKimaru;
		}

		if (tvChangeTimer < 3.0f)
		{
			if (spriteChangeTimer > 0.1f && viewSpriteNum != 1)
			{
				spriteChangeTimer = 0.0f;
				viewSpriteNum++;
			}
			else if (spriteChangeTimer > 0.1f)
			{
				spriteChangeTimer = 0.0f;
				viewSpriteNum--;
			}
		}
		else
		{
			if (!tvChangeSet)
			{
				viewSpriteNum = 2;
				tvChangeSet = true;
			}

			if (spriteChangeTimer > 0.1f && viewSpriteNum != 3)
			{
				spriteChangeTimer = 0.0f;
				viewSpriteNum++;
			}
			else if (spriteChangeTimer > 0.1f)
			{
				spriteChangeTimer = 0.0f;
				viewSpriteNum--;
			}
		}
	}
		break;
	case SceneOpning::SpriteScene::SaruKimaru:
	{
		spriteChangeTimer += elapsedTime;
		sceneChangeTimer += elapsedTime;

		if (spriteChangeTimer > 0.1f && viewSpriteNum != 1)
		{
			spriteChangeTimer = 0.0f;
			viewSpriteNum++;
		}
		else if (spriteChangeTimer > 0.1f)
		{
			spriteChangeTimer = 0.0f;
			viewSpriteNum--;
		}
	}
		break;
	default:
		break;
	}
}

// 描画処理
void SceneOpning::Render()
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

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	float textureWidth = static_cast<float>(homeSprite[0]->GetTextureWidth());
	float textureHeight = static_cast<float>(homeSprite[0]->GetTextureHeight());

	//! 画像表示
	{
		switch (spriteScene)
		{
		case SceneOpning::SpriteScene::Home:
		{
			homeSprite[viewSpriteNum]->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		case SceneOpning::SpriteScene::Tv:
		{
			tvSprite[viewSpriteNum]->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		case SceneOpning::SpriteScene::SaruKimaru:
		{
			saruKimaruSprite[viewSpriteNum]->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		default:
			break;
		}
	}

	//! フェード
	{
		fade->Render(dc);
	}
}
