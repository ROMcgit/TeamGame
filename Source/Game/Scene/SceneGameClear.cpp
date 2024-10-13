#include "SceneGameClear.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneTitle.h"
#include "Input/Input.h"

// 初期化
void SceneGameClear::Initialize()
{
	//! 空
	backGround = std::make_unique<Sprite>("Data/Sprite/背景/空.png");

	//! ショッピングモール
	shopping = std::make_unique <Sprite>("Data/Sprite/背景/ショッピングモール.png");

	//! ショッピングモール(崩壊)
	collapseShopping = std::make_unique<Sprite>("Data/Sprite/背景/ショッピングモール(崩壊).png");

	//! シカ
	sika = std::make_unique<Sprite>("Data/Sprite/背景/シカ2.png");

	//! モンチューブ
	for (int i = 0; i < 2; i++)
	{
		std::string filePath = "Data/Sprite/背景/MonTube" + std::to_string(i + 1) + ".png";
		monTube[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	//! 家の中の画像
	for (int i = 0; i < 2; i++)
	{
		//! 家の中(テレビを見ている)
		std::string filePath = "Data/Sprite/背景/家の中(サル無し)" + std::to_string(i + 1) + ".png";
		house[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	//! テレビ逮捕
	arrestTv = std::make_unique<Sprite>("Data/Sprite/背景/テレビ(サル逮捕).png");

	//! Thank You
	thankYou = std::make_unique<Sprite>("Data/Sprite/背景/ThankYou.png");

	//! フェード
	fade = std::make_unique<Fade>();
}

// 終了化
void SceneGameClear::Finalize()
{
}

// 更新処理
void SceneGameClear::Update(float elapsedTime)
{
	sikaPosY  += 300 * elapsedTime;
	sikaAngle += DirectX::XMConvertToRadians(5000) * elapsedTime;

	fade->Update(elapsedTime);

	if (sceneChange)
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
	

	switch (spriteScene)
	{
	case SpriteScene::SikaDown:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 2.0f)
		{
			sceneChangeTimer = 0.0f;

			//! フェードをセット
			fade->SetFade(DirectX::XMFLOAT3(1, 1, 1),
				1.0f, 0.0f,
				3.0f, 1.0f);

			spriteScene = SpriteScene::CollapseShoppingMall;
		}
	}
		break;
	case SpriteScene::CollapseShoppingMall:

		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 4.0f)
		{
			sceneChangeTimer = 0.0f;

			spriteScene = SpriteScene::MonTube1;
		}

		break;
	case SpriteScene::MonTube1:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 2.0f)
		{
			sceneChangeTimer = 0.0f;

			spriteScene = SpriteScene::MonTube2;
		}
	}
		break;
	case SpriteScene::MonTube2:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 2.0f)
		{
			sceneChangeTimer = 0.0f;

			if (!setMontubefade)
			{
				//! フェードをセット
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					2.0f, 0.0f);

				setMontubefade = true;
			}
			

			if (!fade->GetFade())
			{
				//! フェードをセット
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					1.0f, 0.0f,
					2.0f, 0.0f);

				spriteScene = SpriteScene::House;
			}
			
		}
	}
		break;
	case SpriteScene::House:
	{
		if (!fade->GetFade())
		{
			sceneChangeTimer += elapsedTime;

			if (sceneChangeTimer > 2.0f)
			{
				sceneChangeTimer = 0.0f;

				spriteScene = SpriteScene::Arrest;
			}
		}

		spriteChangeTimer += elapsedTime;

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
	case SpriteScene::Arrest:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 2.0f)
		{
			if (!setArrestFade)
			{
				//! フェードをセット
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					2.0f, 0.0f);

				setArrestFade = true;
			}

			if (!fade->GetFade())
			{
				sceneChangeTimer = 0.0f;

				spriteScene = SpriteScene::ThankYou;
			}
		}
	}
		break;
	case SpriteScene::ThankYou:
	{
		sceneChangeTimer += elapsedTime;

		if (sceneChangeTimer > 0.5f && sceneChangeTimer < 2.0 && thankOpacity < 1.0f)
			thankOpacity += 0.5 * elapsedTime;

		if (sceneChangeTimer > 5.0f && thankOpacity > 0.0f)
			thankOpacity -= 0.5 * elapsedTime;

		if (sceneChangeTimer > 8.0f)
			sceneChange = true;
	}
		break;
	default:
		break;
	}
}

// 描画処理
void SceneGameClear::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.3f, 0.3f, 0.3f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	float textureWidth = static_cast<float>(backGround->GetTextureWidth());
	float textureHeight = static_cast<float>(backGround->GetTextureHeight());

	switch (spriteScene)
	{
	case SpriteScene::SikaDown:
	{
		//! 背景
		backGround->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

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

		textureWidth = static_cast<float>(sika->GetTextureWidth());
		textureHeight = static_cast<float>(sika->GetTextureHeight());

		//! シカ
		sika->Render(dc,
			650, sikaPosY,
			10, 10,
			0, 0,
			textureWidth, textureHeight,
			sikaAngle,
			1, 1, 1, 1);
	}
		break;
	case SpriteScene::CollapseShoppingMall:
	{
		//! ショッピングモール(崩壊)
		collapseShopping->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
		break;
	case SpriteScene::MonTube1:
	{
		//! モンチューブ1
		monTube[0]->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
		break;
	case SpriteScene::MonTube2:
	{
		//! モンチューブ2
		monTube[1]->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
		break;
	case SpriteScene::House:
	{
		//! 家の中
		house[viewSpriteNum]->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
		break;
	case SpriteScene::Arrest:
	{
		//! テレビ逮捕
		arrestTv->Render(dc,
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

	//! フェードを表示
	fade->Render(dc);

	//! Thank You
	if (spriteScene == SpriteScene::ThankYou)
	{
		//! Thank You
		thankYou->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, thankOpacity);
	}
}
