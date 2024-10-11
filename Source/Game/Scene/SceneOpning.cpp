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

	//backGround = std::make_unique<Sprite>(");

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
		house[i] = std::make_unique<Sprite>(filePath.c_str());
	}
	
	//! ショッピングモール
	shopping = std::make_unique < Sprite>("Data/Sprite/背景/ショッピングモール.png");

	//! サル
	for (int i = 0; i < 3; i++)
	{
		std::string filePath = "Data/Sprite/背景/Saru" + std::to_string(i + 1) + ".png";
		saru[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	unko = std::make_unique<Sprite>("Data/Sprite/背景/うんち.png");

	//! シカ
	for (int i = 0; i < 2; i++)
	{
		std::string filePath = "Data/Sprite/背景/シカ" + std::to_string(i + 1) + ".png";
		sika[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	//! シカ(最終形態)
	for (int i = 0; i < 300; i++)
	{
		sikaFinal[i] = std::make_unique<Sprite>("Data/Sprite/背景/シカ2.png");

		sikaPos[i] = { 280, 80 };

		DirectX::XMFLOAT2 posPower = { 
			(float)(rand() % 10 + 1 * (rand() % 2 == 1 ? -1 : 1)) * 100,
			(float)(rand() % 10 + 1 * (rand() % 2 == 1 ? -1 : 1)) * 100
		};

		if (i >= 0 && i < 10)
			posPower.x *= -1;

		//! シカの移動スピード
		sikaPosPower[i] = { posPower.x, posPower.y };
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
	fade->Update(elapsedTime);

	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_A && !setFade)
		doFade = true;

	if (doFade)
	{
		if (!setFade)
		{
			// フェードをセット
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				3.0f);

			setFade = true;
		}
	}

	//! フェードが終わったらシーン遷移
	if (setFade && !fade->GetFade())
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

		//! 画像シーンを切り替える
		if (sceneChangeTimer > 3.0f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::SaruRan;
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
	case SpriteScene::SaruRan:
	{
		sceneChangeTimer += elapsedTime;
		saruScalePlusTimer += elapsedTime;

		//! 画像シーンを切り替える
		if (sceneChangeTimer > 3.5f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::ShoppingMall;
		}

		if (saruScalePlusTimer > 1.0f)
		{
			saruScale.x += 500 * elapsedTime;
			saruScale.y += 515 * elapsedTime;

			saruPos.x -= 200 * elapsedTime;
			saruPos.y -= 350 * elapsedTime;
		}
	}
		break;
	case SpriteScene::ShoppingMall:
	{
		sceneChangeTimer += elapsedTime;
		sikaMoveTimer    += elapsedTime;

		if (sceneChangeTimer > 8.5f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::SaruKireru;
		}

		if (shoppingPosY > 0)
		{
			shoppingPosY -= 100 * elapsedTime;
		}
		//! 大きさ制限
		if (shoppingPosY < 0)
			shoppingPosY = 0;

		if (sikaMoveTimer > 7.0f)
		{
			//! シカのX座標
			if (sikaPosX > 300)
				sikaPosX -= 2000 * elapsedTime;

			if (sikaPosX < 300)
				sikaPosX = 300;
		}
	}
		break;
	case SpriteScene::SaruKireru:
	{
		sceneChangeTimer += elapsedTime;
		unkoScaleTimer   += elapsedTime;

		if (sceneChangeTimer > 3.5f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::UnkoAttack;
		}

		//! うんこを出す
		if (unkoScaleTimer > 1.5f)
		{
			unkoPos.x -= 550 * elapsedTime;
			unkoPos.y -= 650 * elapsedTime;

			unkoScale.x += 1200 * elapsedTime;
			unkoScale.y += 1420 * elapsedTime;
			unkoAngle += DirectX::XMConvertToRadians(1500) * elapsedTime;
		}
	}
		break;
	case SpriteScene::UnkoAttack:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 1.6f)
		{
			//! フェードをセット
			fade->SetFade(DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f),
				1.0f, 0.0f,
				1.0f, 1.0f
			);

			sceneChangeTimer = 0;

			spriteScene = SpriteScene::UnkoHit;
		}


		unkoPos.x += 720 * elapsedTime;
		unkoPos.y += 850 * elapsedTime;
		unkoAngle += DirectX::XMConvertToRadians(1500) * elapsedTime;

		//! うんこを小さくする
		if (unkoScale.x > 0.0f)
			unkoScale.x -= 1500 * elapsedTime;
		if (unkoScale.y > 0.0f)
			unkoScale.y -= 1820 * elapsedTime;
	}
		break;
	case SpriteScene::UnkoHit:
	{
		sceneChangeTimer += elapsedTime;

		if(sceneChangeTimer > 6.0f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::SaruOdoroku;
		}
	}
		break;
	case SpriteScene::SaruOdoroku:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 2.0f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::SikaRash;
		}
	}
		break;
	case SpriteScene::SikaRash:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 2.0f)
		{
			sceneChangeTimer = 0.0f;
			spriteScene = SpriteScene::SaruOdoroku2;
		}

		for (int i = 0; i < 300; i++)
		{
			sikaPos[i].x += sikaPosPower[i].x * elapsedTime;
			sikaPos[i].y += sikaPosPower[i].y * elapsedTime;

			sikaStopTimer[i] += elapsedTime;

			if (sikaStopTimer[i] > 0.5f)
				sikaPosPower[i] = { 0, 0 };
		}
	}
	break;
	case SpriteScene::SaruOdoroku2:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 2.0f)
			doFade = true;
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
			//textureWidth = static_cast<float>(backGround->GetTextureWidth());
			//textureHeight = static_cast<float>(backGround->GetTextureHeight());

			////! 背景
			//backGround->Render(dc,
			//	0, 0,
			//	screenWidth, screenHeight,
			//	0, 0,
			//	textureWidth, textureHeight,
			//	0,
			//	1, 1, 1, 1);

			textureWidth = static_cast<float>(house[0]->GetTextureWidth());
			textureHeight = static_cast<float>(house[0]->GetTextureHeight());

			if (saruScalePlusTimer > 1.02f)
			{
				//! 家
				house[1]->Render(dc,
					0, 0,
					screenWidth, screenHeight,
					0, 0,
					textureWidth, textureHeight,
					0,
					1, 1, 1, 1);
			}
			else
			{
				//! 家
				house[0]->Render(dc,
					0, 0,
					screenWidth, screenHeight,
					0, 0,
					textureWidth, textureHeight,
					0,
					1, 1, 1, 1);
			}
			

			textureWidth = static_cast<float>(saru[0]->GetTextureWidth());
			textureHeight = static_cast<float>(saru[0]->GetTextureHeight());

			//! サル(通常)
			saru[0]->Render(dc,
				saruPos.x, saruPos.y,
				saruScale.x, saruScale.y,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::ShoppingMall:
		{
			//textureWidth = static_cast<float>(backGround->GetTextureWidth());
			//textureHeight = static_cast<float>(backGround->GetTextureHeight());

			////! 背景
			//backGround->Render(dc,
			//	0, 0,
			//	screenWidth, screenHeight,
			//	0, 0,
			//	textureWidth, textureHeight,
			//	0,
			//	1, 1, 1, 1);

			textureWidth = static_cast<float>(shopping->GetTextureWidth());
			textureHeight = static_cast<float>(shopping->GetTextureHeight());

			//! ショッピングモール
			shopping->Render(dc,
				0, shoppingPosY,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(sika[0]->GetTextureWidth());
			textureHeight = static_cast<float>(sika[0]->GetTextureHeight());

			//! シカ
			sika[0]->Render(dc,
				sikaPosX, 0,
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
				330, 80,
				588, 632,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(unko->GetTextureWidth());
			textureHeight = static_cast<float>(unko->GetTextureHeight());

			//! うんこ
			unko->Render(dc,
				unkoPos.x, unkoPos.y,
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
				unkoPos.x, unkoPos.y,
				unkoScale.x, unkoScale.y,
				0, 0,
				textureWidth, textureHeight,
				unkoAngle,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::UnkoHit:
		{
			textureWidth = static_cast<float>(sika[1]->GetTextureWidth());
			textureHeight = static_cast<float>(sika[1]->GetTextureHeight());

			//! シカ(最終形態)
			sika[1]->Render(dc,
				150, 0,
				924, 710,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::SaruOdoroku:
		{
			textureWidth = static_cast<float>(saru[2]->GetTextureWidth());
			textureHeight = static_cast<float>(saru[2]->GetTextureHeight());

			//! サル(驚く)
			saru[2]->Render(dc,
				330, 80,
				588, 632,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
			break;
		case SpriteScene::SikaRash:
		{
			textureWidth = static_cast<float>(sika[1]->GetTextureWidth());
			textureHeight = static_cast<float>(sika[1]->GetTextureHeight());

			//! シカ(最終形態)
			sika[1]->Render(dc,
				280, 80,
				724, 510,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			textureWidth = static_cast<float>(sikaFinal[0]->GetTextureWidth());
			textureHeight = static_cast<float>(sikaFinal[0]->GetTextureHeight());

			for (int i = 0; i < 300; i++)
			{
				sikaFinal[i]->Render(dc,
					sikaPos[i].x, sikaPos[i].y,
					624, 410,
					0, 0,
					textureWidth, textureHeight,
					0,
					1, 1, 1, 1);
			}
		}
			break;
		case SpriteScene::SaruOdoroku2:
		{
			textureWidth = static_cast<float>(saru[2]->GetTextureWidth());
			textureHeight = static_cast<float>(saru[2]->GetTextureHeight());

			//! サル(驚く)
			saru[2]->Render(dc,
				100, 0,
				1088,1132,
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
