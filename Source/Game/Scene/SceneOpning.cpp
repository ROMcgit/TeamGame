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

	//! 家
	for (int i = 0; i < 2; i++)
	{
		std::string filePath = "Data/Sprite/背景/家" + std::to_string(i + 1) + ".png";
		house[i] = std::make_unique<Sprite>(filePath);
	}
	
	//! ショッピングモール
	shopping = std::make_unique < Sprite>("Data/Sprite/背景/ショッピングモール.png");

	//! サル
	for (int i = 0; i < 3; i++)
	{
		std::string filePath = "Data/Sprite/背景/Saru" + std::to_string(i + 1) + ".png";
		saru[i] = std::make_unique<Sprite>(filePath);
	}

	//! シカ
	for (int i = 0; i < 2; i++)
	{
		std::string filePath = "Data/Sprite/背景/シカ" + std::to_string(i + 1) + ".png";
		sika[i] = std::make_unique<Sprite>(filePath);
	}
	
	//! シカ(最終形態)
	for (int i = 0; i < 100; i++)
	{
		sikaFinal[i] = std::make_unique<Sprite>("Data/Sprite/背景/シカ2.png");
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
		if (sceneChangeTimer > 2.0f)
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
		if (sceneChangeTimer > 6.0f)
		{
			viewSpriteNum = 0;
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::SaruKimaru;
		}

		if (tvChangeTimer < 2.5f)
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

			if (spriteChangeTimer > 0.15f && viewSpriteNum != 3)
			{
				spriteChangeTimer = 0.0f;
				viewSpriteNum++;
			}
			else if (spriteChangeTimer > 0.15f)
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
		case SpriteScene::Home:
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
		case SpriteScene::Tv:
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
		case SpriteScene::SaruKimaru:
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
		case SpriteScene::SaruRan:
		{
			textureWidth = static_cast<float>(saru[0]->GetTextureWidth());
			textureHeight = static_cast<float>(saru[0]->GetTextureHeight());

			//! サル(通常)
			saru[0]->Render(dc,
				0, 0,
				saruScale.x, saruScale.y,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::ShoppingMall:
		{
			textureWidth = static_cast<float>(shopping->GetTextureWidth());
			textureHeight = static_cast<float>(shopping->GetTextureHeight());

			//! ショッピングモール
			shopping->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(sika[0]->GetTextureWidth());
			textureHeight = static_cast<float>(sika[0]->GetTextureHeight());

			//! シカ
			sika[0]->Render(dc,
				300, 0,
				650, 720,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::SaruKireru:
		{
			textureWidth = static_cast<float>(saru[1]->GetTextureWidth());
			textureHeight = static_cast<float>(saru[1]->GetTextureHeight());

			//! サル(キレる)
			saru[1]->Render(dc,
				0, 0,
				328, 372,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(unko->GetTextureWidth());
			textureHeight = static_cast<float>(unko->GetTextureHeight());

			//! うんこ
			unko->Render(dc,
				0, 0,
				unkoScale.x, unkoScale.y,
				0, 0,
				textureWidth, textureHeight,
				unkoAngle,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::UnkoAttack:
		{
			textureWidth = static_cast<float>(sika[0]->GetTextureWidth());
			textureHeight = static_cast<float>(sika[0]->GetTextureHeight());

			//! シカ
			sika[0]->Render(dc,
				300, 0,
				650, 720,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(unko->GetTextureWidth());
			textureHeight = static_cast<float>(unko->GetTextureHeight());

			//! うんこ
			unko->Render(dc,
				0, 0,
				unkoScale.x, unkoScale.y,
				0, 0,
				textureWidth, textureHeight,
				unkoAngle,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::UnkoHit:
		{
			textureWidth = static_cast<float>(kemuri->GetTextureWidth());
			textureHeight = static_cast<float>(kemuri->GetTextureHeight());

			kemuri->Render(dc,
				0, 0,
				screenWidth, screenWidth,
				0, 0,
				textureWidth, textureHeight,
				unkoAngle,
				1, 1, 1, kemuriOpacity);
		}
			break;
		case SpriteScene::SaruOdoroku:
		{
			textureWidth = static_cast<float>(sika[1]->GetTextureWidth());
			textureHeight = static_cast<float>(sika[1]->GetTextureHeight());

			//! シカ(最終形態)
			sika[1]->Render(dc,
				0, 0,
				screenWidth, screenWidth,
				0, 0,
				textureWidth, textureHeight,
				unkoAngle,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::SikaRash:
		{
			textureWidth = static_cast<float>(sikaFinal[0]->GetTextureWidth());
			textureHeight = static_cast<float>(sikaFinal[0]->GetTextureHeight());

			for (int i = 0; i < 100; i++)
			{
				sikaFinal[i]->Render(dc,
					0, 0,
					screenWidth, screenWidth,
					0, 0,
					textureWidth, textureHeight,
					unkoAngle,
					1, 1, 1, 1);
			}
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
