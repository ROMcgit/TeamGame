#include "Pause.h"
#include "Graphics.h"
#include "Fade.h"

#include "Input/Input.h"
#include "Game/Scene/SceneGameSelect.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneManager.h"
#include "Game/Camera/CameraController.h"

#include "Audio/BgmManager.h"
#include "Audio/SoundManager.h"

// コンストラクタ
Pause::Pause()
{
	//! 効果音
	SoundManager& sound = SoundManager::Instance();
	sound.LoadSound("表示", "Data/Audio/Sound/Decision.wav");
	sound.LoadSound("タイトルへ戻る", "Data/Audio/Sound/Cancel.wav");

	pauseBG = std::make_unique<Sprite>("Data/Sprite/Pause/Pause.png");

	//! ポーズ画面選択
	for (int i = 0; i < 2; i++)
	{
		std::string filePath = "Data/Sprite/Pause/PauseSelect" + std::to_string(i) + ".png";

		pauseSelectImage[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	pauseExplanation = std::make_unique<Sprite>("Data/Sprite/Pause/PauseExplanation.png");

	//! フェード
	fade = std::make_unique<Fade>();
}

Pause::~Pause()
{
}

// 更新処理
void Pause::Update(float elapsedTime)
{
	//! デバッグカメラじゃないなら
	if (!CameraController::debugCamera)
	{
		//! フェードの更新処理
		fade->Update(elapsedTime);

		//! ポーズ画面の処理
		UpdatePause(elapsedTime);
	}
}

// 描画処理
void Pause::Render(ID3D11DeviceContext* dc, Graphics& graphics)
{
	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	float textureWidth = static_cast<float>(pauseExplanation->GetTextureWidth());
	float textureHeight = static_cast<float>(pauseExplanation->GetTextureHeight());

	float opacity = 1.0f - pauseOpacity;

	//! ポーズ画面の説明
	pauseExplanation->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		0.8f, 0.8f, 0.8f, opacity);

	textureWidth = static_cast<float>(pauseBG->GetTextureWidth());
	textureHeight = static_cast<float>(pauseBG->GetTextureHeight());

	//! ポーズ画面の背景
	pauseBG->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		0.5f, 0.5f, 0.5f, pauseOpacity);

/************************************************************************************************************/

	//! ポーズ画面のボード
	for (int i = 0; i < 2; i++)
	{
		textureWidth = static_cast<float>(pauseSelectImage[i]->GetTextureWidth());
		textureHeight = static_cast<float>(pauseSelectImage[i]->GetTextureHeight());

		if(i + 1 == pauseSelect)
		{
			pauseSelectImage[i]->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, pauseOpacity);
		}
		else
		{
			pauseSelectImage[i]->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				0.5f, 0.5f, 0.5f, pauseOpacity);
		}
	}

	//! 特殊な描画処理
	{
		//! フェードの描画処理
		fade->Render(dc, graphics);
	}
}

// ポーズ画面の処理
void Pause::UpdatePause(float elapsedTime)
{
	//! ボタン情報のインスタンス取得
	GamePad& gamePad = Input::Instance().GetGamePad();

	//! 効果音
	SoundManager& sound = SoundManager::Instance();

	//! ポーズ画面じゃ無いなら
	if (!pause)
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_START && pauseOpacity <= 0.0f)
		{
			//! ポーズ画面にする
			sound.PlaySound("表示");
			pause = true;
		}

		//! 不透明度が0.0fより大きいなら、数値を下げる
		if (pauseOpacity > 0.0f)
			pauseOpacity -= pauseOpacitySpeed * elapsedTime;
		else
		{
			pauseSelect = 1;
		}
	}
	//! ポーズ画面中なら
	else
	{
		//! 画面が表示仕切ったら、操作を受けつける
		if (pauseOpacity >= 1.0f)
		{
			//! ボタンの入力状態
			int buttonState = gamePad.GetButtonDown();

			if (!setFade)
			{
				switch (buttonState)
				{
				case GamePad::BTN_START:
				case GamePad::BTN_B:
				{
					//! ポーズ画面を終了
					sound.PlaySound("表示");
					pause = false;
				}
				break;
				case GamePad::BTN_UP:
				{
					if (pauseSelect == 2)
					{
						pauseSelect--;
					}

				}
				break;
				case GamePad::BTN_DOWN:
				{
					if (pauseSelect == 1)
					{
						pauseSelect++;
					}
				}
				break;
				case GamePad::BTN_A:
				{
					//! 選択によって処理を変える
					switch (pauseSelect)
					{
					case 1:
					{
						//! ポーズ画面を終了
						sound.PlaySound("表示");
						pause = false;
					}
					break;
					case 2:
					{
						//! 効果音
						sound.PlaySound("タイトルへ戻る");

						//! フェードを設定したか
						fade->SetFade(DirectX::XMFLOAT3(1, 1, 1),
							0.0f, 1.0f,
							0.5f, 0.2f);

						setFade = true;
					}
					break;
					default:
						break;
					}
				}
				break;
				default:
					break;
				}
			}
			//! フェードを設定したなら
			else if (setFade && !fade->GetFade())
			{
				//! シーン切り替え処理
				SceneChange();
			}
		}

		//! 不透明度が1.0f未満なら、数値を上げる
		if (pauseOpacity < 1.0f)
			pauseOpacity += pauseOpacitySpeed * elapsedTime;
	}
}

// シーン切り替え処理
void Pause::SceneChange()
{
	BgmManager& bgm = BgmManager::Instance();

	bgm.UnloadBgm("おにごっこ");
	bgm.UnloadBgm("だるまさんが転んだ");
	bgm.UnloadBgm("寸止め");
	bgm.UnloadBgm("空飛ぶ風船割り");
	bgm.UnloadBgm("おっさん叩き");
	bgm.UnloadBgm("足場渡り");

	std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

	// シーンマネージャーにローディングシーンへの切り替えを指示
	SceneManager::Instance().ChangeScene(std::move(loadingScene));
}