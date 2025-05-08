#include "Graphics/Graphics.h"
#include "Game/Scene/G2_Sundome_Result.h"
#include "Game/Scene/G2_Sundome.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Fade.h"
#include "Other/Easing.h"

// 初期化
void G2_Sundome_Result::Initialize()
{
	// スプライト初期化
	backGround = std::make_unique<Sprite>("Data/Sprite/Title.png");

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);

	for(int i = 0; i < 4; i++)
	{
		text[i] = std::make_unique<Text>();
	}

	float screeenWidth = Graphics::Instance().GetScreenWidth();
	for (int i = 0; i < 4; i++)
	{
		textPos[i].x = screeenWidth * 1.1f;
	}

	float screenHeight = Graphics::Instance().GetScreenHeight();
	textPos[0].y = screenHeight * 0.2f;
	textPos[1].y = screenHeight * 0.4f;
	textPos[2].y = screenHeight * 0.6f;
	textPos[3].y = screenHeight * 0.8f;

	for (int i = 0; i < 4; i++)
		textPos[i].x = startTextPosX[i];
}

// 終了化
void G2_Sundome_Result::Finalize()
{
}

// 更新処理
void G2_Sundome_Result::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	fade->Update(elapsedTime);

	// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y;
	if ((gamePad.GetButtonDown() & anyButton && !setFade && !fade->GetFade()) && directorFinish)
	{
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			0.5f, 0.2f);

		setFade = true;
	}
	else if (setFade && !fade->GetFade())
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// 描画処理
void G2_Sundome_Result::Render()
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
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		// スコア
		for (int i = 0; i < 3; i++)
		{
			text[i]->RenderThousand(dc, false,
				G2_Sundome::score[i],
				false, false, false,
				textPos[i].x, textPos[i].y);
		}

		//! 合計スコア
		text[3]->RenderThousand(dc, false,
			totalScore,
			false, false, false,
			textPos[3].x, textPos[3].y,
			12.0f, 12.0f, 0,
			30.0f);

		fade->Render(dc, graphics);
	}

#ifndef _DEBUG

	{
		if (ImGui::Begin("Debug"))
		{
			for(int i = 0; i < 4; i++)
			{
				std::string name = "TextPos" + std::to_string(i);
				ImGui::DragFloat2(name.c_str(), &textPos[i].x, 0.5f);
			}
		}
	}

#endif // !_DEBUG
}

// スコアの演出処理
void G2_Sundome_Result::DirectorScore(float elapsedTime)
{
	if (directorFinish) return;

	switch (directorStep)
	{
	case 0:

		directorTime += elapsedTime;

		if (directorTime > 2.0f)
		{
			directorTime = 0.0f;
			directorStep++;
		}

		break;
	//! textPos[textNum].xの変更
	case 1:

		float screeenWidth = Graphics::Instance().GetScreenWidth();

		float t = directorTime / 0.8f;

		if(t < 1.0f)
		{
			textPos[textNum].x = Easing::EaseOut(startTextPosX[0], screeenWidth * 0.7f, t);
		}
		else
		{
			directorTime = 0.0f;
			directorStep++;
			textNum++;
		}
		break;
	//! 待ち時間
	case 2:

		directorTime += elapsedTime;

		if (directorTime > 0.3f)
		{
			directorTime = 0.0f;
			if (textNum >= 3)
			{
				directorStep++;
			}
			else
				directorStep = 1;
		}
		break;
	case 3:

		directorTime += elapsedTime;

		if (directorTime > 1.0f)
		{
			directorFinish = true;
		}

		break;
	default:
		break;
	}
}