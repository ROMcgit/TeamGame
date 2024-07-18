#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"

// 初期化
void SceneTitle::Initialize()
{
	// スプライト初期化
	sprite = new Sprite("Data/Sprite/タイトル.png");
	fadeIn = std::make_unique<Sprite>("Data/Sprite/画面フェードイン.png");

	mozi[0] = std::make_unique<Sprite>("Data/Sprite/置いてきた.png");
	mozi[1] = std::make_unique<Sprite>("Data/Sprite/帰る.png");
	
	Audio& audioManager = Audio::Instance();
	sound[0] = audioManager.LoadAudioSource("Data/Audio/年寄り.wav");
	sound[1] = audioManager.LoadAudioSource("Data/Audio/キャァァァァァ(鈍足).wav");
	sound[2] = audioManager.LoadAudioSource("Data/Audio/決定音.wav");

	bgm = audioManager.LoadAudioSource("Data/Audio/opening.wav");

	yazirusi = std::make_unique<Sprite>("Data/Sprite/矢印.png");

	SceneTitle& title = SceneTitle::Instance();
	title.score = 0;
	title.scorePlus = 0;
	title.scorePlusResetTime = 0;
	title.combo = 0;
	title.comboResetTime = 0;
}

// 終了化
void SceneTitle::Finalize()
{
	// スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
	if (bgm)
	{
		bgm->Play(true, 0.7f);
	}

	fadeInView -= 0.005f;

	if (fadeInView < 0)
	{
		fadeInView = 0;
		sceneOK = true;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	if (sceneOK == true)
	{
		// シーン遷移
		if ((gamePad.GetButtonDown() & GamePad::BTN_START ||
			gamePad.GetButtonDown() & GamePad::BTN_A) && select == 0)
		{
			sound[2]->Play(false, 1.0f);
			fadeOut = true;
		}
		if (fadeOut == true)
		{
			fadeOutView += 0.005f;
		}

		if (fadeOutView > 1)
		{
			bgm->Stop();
			SceneLoading* loadingScene = new SceneLoading(new SceneGame);

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(loadingScene);
		}

		// チュートリアル
		if ((gamePad.GetButtonDown() & GamePad::BTN_START ||
			gamePad.GetButtonDown() & GamePad::BTN_A)
			&& select == 1)
		{
			sound[0]->Play(false, 1.0f);
			moziView = true;
		}

		// 帰さない
		if ((gamePad.GetButtonDown() & GamePad::BTN_START ||
			gamePad.GetButtonDown() & GamePad::BTN_A) && select == 2)
		{
			sound[1]->Play(false, 1.0f);
			moziView = true;
		}

		if (moziView == true)
		{
			waitTime++;
		}

		if (waitTime > 300 && select == 1)
		{
			waitTime = 0;
			moziView = false;
		}
		// シーン遷移
		else if (waitTime > 300 && select == 2)
		{
			bgm->Stop();
			SceneLoading* loadingScene = new SceneLoading(new SceneGame);

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(loadingScene);
		}

		// 選択処理
		if (moziView == false &&
			gamePad.GetButtonDown() & GamePad::BTN_UP) select--;
		else if
			(moziView == false &&
				gamePad.GetButtonDown() & GamePad::BTN_DOWN) select++;

		// 数値を超えないように
		if (select < 0) select = 0;
		if (select > 2) select = 2;
	}
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
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		// タイトルスプライト描画
		if (moziView == true && select == 2)
		{
			sprite->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 0, 0, 1);
		}
		else
			sprite->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);


		if (select == 1 && moziView == true)
		{
			mozi[0]->Render(dc,
				0, 10,
				screenWidth, screenHeight,
				0, 0,
				1280, 720,
				0,
				1, 1, 1, 1);
		}

		if (select == 2 && moziView == true)
		{
			mozi[1]->Render(dc,
				40, 0,
				screenWidth, screenHeight,
				0, 0,
				1280, 720,
				0,
				1, 1, 1, 1);
		}

		switch (select)
		{
		case 0:
			yazirusi->Render(dc,
				6, 48,
				screenWidth, screenHeight,
				0, 0,
				1280, 720,
				0,
				1, 1, 1, 1);
			break;
		case 1:
			yazirusi->Render(dc,
				-36, 130,
				screenWidth, screenHeight,
				0, 0,
				1280, 720,
				0,
				1, 1, 1, 1);
			break;
		case 2:
			yazirusi->Render(dc,
				15, 225,
				screenWidth, screenHeight,
				0, 0,
				1280, 720,
				0,
				1, 1, 1, 1);
			break;
		default:
			break;
		}
	}

	// フェードアウト
	fadeIn->Render(dc,
		0, 0,
		2230, 1750,
		0, 0,
		2230, 1750,
		0,
		0, 0, 0, fadeInView);

	// フェードアウト
	fadeIn->Render(dc,
		0, 0,
		2230, 1750,
		0, 0,
		2230, 1750,
		0,
		0, 0, 0, fadeOutView);
}