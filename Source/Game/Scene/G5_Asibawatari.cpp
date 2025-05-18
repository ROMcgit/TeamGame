#include "Graphics/Graphics.h"
#include "G5_Asibawatari.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G5_StageAsibawatari_Normal.h"
#include "Game/Stage/G5_StageAsibawatari_Normal_Horizontal.h"
#include "Game/Stage/G5_StageAsibawatari_Normal_Vertical.h"
#include "Game/Stage/G5_StageAsibawatari_Trap.h"
#include "Game/Stage/G5_StageAsibawatari_Goal.h"
#include "Game/Stage/StageMoveFloor.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "G5_Asibawatari_GameOver.h"
#include "G5_Asibawatari_Clear.h"
#include "Audio/BgmManager.h"

bool G5_Asibawatari::gameClear = false;

// 初期化
void G5_Asibawatari::Initialize()
{
	gameClear = false;

	movieScene = true;

	Graphics& graphics = Graphics::Instance();
	graphics.GetEnvironmentMap()->Load("Data/Environment/SF_Night.hdr");
	graphics.GetEnvironmentMap()->Set(15);

	//! 空
	sky = std::make_unique<Sky>();

	ID3D11Device* device = graphics.GetDevice();
	float screenWidth = graphics.GetScreenWidth();
	float screenHeight = graphics.GetScreenHeight();

	// レンダーターゲット
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// シャドウマップ初期化
	shadowMap.Initialize();

/********************************************************************************/

		// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	std::unique_ptr<G5_StageAsibawatari_Normal> stageMain = std::make_unique<G5_StageAsibawatari_Normal>();
	stageMain->SetMoveSpeed(0.0f);
	stageManager.Register(std::move(stageMain));

	// プレイヤー初期化
	player = std::make_unique<Player5_AsibaWatari>();
	player->SetPosition(DirectX::XMFLOAT3(0, 5.0f, 0));
	player->SetAngleY(DirectX::XMConvertToRadians(180));
	//player->SetGravity(0.0f);

	// カメラ初期設定
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	//! ポストエフェクトの設定(一回だけ)フラグを初期化
	camera.SetPostEffectStatusOnceUnlock();

	//カメラコントローラー初期化
	cameraController = std::make_unique <CameraController>();
	cameraController->SetTarget(DirectX::XMFLOAT3(0, 0.5f, 0.0f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(47), 0, 0));
	cameraController->SetRange(48.5f);

	// 背景
	backGround = std::make_unique<Sprite>();

	//! フェード
	fade = std::make_unique<Fade>();
	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);

	//! ポーズ画面
	pause = std::make_unique<Pause>();

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("足場渡り", "Data/Audio/Bgm/12.Asibawatari.wav");
	bgm.PlayBgm("足場渡り", 0.5f);
}

// 終了化
void G5_Asibawatari::Finalize()
{
	//! ステージを破棄する
	StageManager::Instance().Clear();

	//! 敵を全て破棄する
	EnemyManager::Instance().Clear();

	//! エフェクトを全て破棄する
	EffectManager::Instance().Cleanup();
}

// 更新処理
void G5_Asibawatari::Update(float elapsedTime)
{
	//! ポーズ画面の更新処理
	pause->Update(elapsedTime);

	if(pause->GetPauseOpacity() <= 0.0f)
	{
		//! フェードの更新処理
		fade->Update(elapsedTime);

		//! ムービー更新処理
		UpdateMovie(elapsedTime);

		// カメラコントローラー更新処理
		Camera::Instance().Update(elapsedTime);
		cameraController->Update(elapsedTime);

		// ステージ更新処理
		if (!gameClear)
			StageManager::Instance().Update(elapsedTime);

		//! ステージ生成処理
		NewStage(elapsedTime);

		// プレイヤー更新処理
		if (!gameClear)
			player->Update(elapsedTime);

		// エネミー更新処理
		EnemyManager::Instance().Update(elapsedTime);

		// エフェクト更新処理
		EffectManager::Instance().Update(elapsedTime);

		//! シーン切り替え処理
		SceneChange();
	}
}

// 描画処理
void G5_Asibawatari::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 5.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 4.0f, 25.0f, 9.0f };

	//! フォグ
	fogStart = 2000.0f;
	fogEnd = 2100.0f;

	Graphics& graphics = Graphics::Instance();

	DrawingSettings(graphics);

	//! シャドウマップ
	{
		// ポーズ画面じゃないなら
		if (pause->GetPauseOpacity() < 1.0f)
		{
			//! シャドウマップ開始
			shadowMap.Begin(rc);
			{
				Shader* shadowMapShader = graphics.GetShadowMapShader();
				shadowMapShader->Begin(dc, rc);

				//エネミー描画
				EnemyManager::Instance().Render(dc, shadowMapShader);
				// プレイヤー描画
				player->Render(dc, shadowMapShader, true);

				shadowMapShader->End(dc);
			}
			//! シャドウマップ終了
			shadowMap.End();
		}
	}

	//! レンダーターゲット
	{
		//! レンダーターゲットに描画開始
		renderTarget->Begin();
	}

	//! 2Dスプライト
	{
		// 深度を無効にする
		ID3D11DepthStencilState* depthDisabledState = graphics.GetDepthDisabledState();
		dc->OMSetDepthStencilState(depthDisabledState, 0);

		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(backGround->GetTextureWidth());
		float textureHeight = static_cast<float>(backGround->GetTextureHeight());

		//! 背景描画
		backGround->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			backGroundColor.x, backGroundColor.y, backGroundColor.z, 1);

		ID3D11DepthStencilState* depthEnabledState = graphics.GetDepthEnabledState();
		dc->OMSetDepthStencilState(depthEnabledState, 0);
	}

	// 3Dモデル描画
	{
		// ポーズ画面じゃないなら
		if (pause->GetPauseOpacity() < 1.0f)
		{
			//! スカイマップ
			{
				Shader* skyShader = graphics.GetSkydomeShader();
				skyShader->Begin(dc, rc);

				sky->Render(dc, skyShader);

				skyShader->End(dc);
			}

			Shader* shader = graphics.GetDefaultLitShader();
			shader->Begin(dc, rc);
			// ステージ描画
			StageManager::Instance().Render(dc, shader);

			// プレイヤー描画
			player->Render(dc, shader);

			// カメラの位置を描画
			cameraController->RenderCameraTarget(dc, shader);

			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);
			shader->End(dc);
		}
	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	//! シェーダーを出す
	{
		//! レンダーターゲットへ描画終了
		renderTarget->End();
		//! スクリーンをポストエフェクトシェーダーで描画
		Camera::Instance().CreatePostEffect();
		Camera::Instance().SetPostEffectStatusOnce();
		//! スクリーンをポストエフェクトシェーダーで描画
		renderTarget->Render();
	}

#ifndef _DEBUG

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		player->DrawDebugPrimitive();

		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

#endif // !_DEBUG

	{
		//! フェードの描画処理
		fade->Render(dc, graphics);
	}

#ifndef _DEBUG

	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::ColorEdit3("BGColor", &backGroundColor.x);
			ImGui::InputFloat3("BGColorNum", &backGroundColor.x);

			// プレイヤーデバッグ描画
			player->DrawDebugGUI();
			//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-----------------------------------------------------------------------------------------------------//
						// !カメラのデバッグ描画
			if (ImGui::TreeNode("Cameras"))
			{
				//-------------------------------------------------------------------------------------------------------
				ImGui::Spacing(); // 一行空ける
				ImGui::Separator(); // セクションの間に区切り線を表示
				ImGui::Spacing(); // 一行空ける
				//-------------------------------------------------------------------------------------------------------
				Camera::Instance().DrawDebugGUI(); // ! カメラ

				ImGui::Spacing(); // 一行空ける
				ImGui::Separator(); // セクションの間に区切り線を表示
				ImGui::Spacing(); // 一行空ける

				cameraController->DrawDebugGUI(); // !カメラコントローラー
				ImGui::TreePop();
			}
			//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-----------------------------------------------------------------------------------------------------//
			EnemyManager::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}

#endif // !_DEBUG
}

// ムービー更新処理
void G5_Asibawatari::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	player->SetMovieTime(0.1f);

	switch (movieStep)
	{
	case 0:

		movieTime += elapsedTime;

		if (movieTime > 2.0f)
		{
			std::unique_ptr<Stage>& stage = StageManager::Instance().GetStage(0);
			stage->SetMoveSpeed(2.0f);

			movieScene = false;
		}

		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}

// ステージ生成処理
void G5_Asibawatari::NewStage(float elapsedTime)
{
	if (movieScene || newStageFinish) return;

	gameTimer += elapsedTime;

	float posX = 50.0f;

	//! 位置Zは-18～18

	switch (stageStep)
	{
		//! 通常
	case 0:

		if (gameTimer > 2.0f)
		{
			//! 通常
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(normal));

			gameTimer = 0.0f;
			stageStep++;
		}
		break;
	case 1:

		if (gameTimer > 1.8f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					//! 横長
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float posZ = i == 0 ? 14 : -14;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
		break;
	case 2:

		if (gameTimer > 10.0f)
		{
			//! 通常
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);
			//normal->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(normal));

			//! 縦長
			std::unique_ptr<G5_StageAsibawatari_Normal_Vertical> vertical = std::make_unique<G5_StageAsibawatari_Normal_Vertical>();
			vertical->SetPositionX(posX + 17);
			vertical->SetPositionZ(0);
			vertical->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(vertical));

			gameTimer = 0.0f;
			stageStep++;
		}
		break;
	case 3:
	{
		if (gameTimer > 6.3f)
		{
			//! 通常
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(10);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.075f, 0.1f, 0.075f));

			StageManager::Instance().Register(std::move(normal));

			//! トラップ
			std::unique_ptr<G5_StageAsibawatari_Trap> trap = std::make_unique<G5_StageAsibawatari_Trap>();
			trap->SetPositionX(posX);
			trap->SetPositionZ(-10);
			trap->SetMoveSpeed(5.0f);
			trap->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(trap));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
		break;
	case 4:
	{
		if (gameTimer > 4.0f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					//! 横長
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float num = j == 0 ? 12 : 6;
					float posZ = i == 0 ? num : -num;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 5:
	{
		if (gameTimer > 9.5f)
		{
			//! 通常
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.075f, 0.1f, 0.075f));

			StageManager::Instance().Register(std::move(normal));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 6:
	{
		if (gameTimer > 4.0f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					//! 横長
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float num = j == 0 ? 6 : 12;
					float posZ = i == 0 ? num : -num;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 7:
	{
		if (gameTimer > 8.0f)
		{
			//! 通常
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX + 17);
			normal->SetPositionZ(0);
			normal->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(normal));

			//! 縦長
			std::unique_ptr<G5_StageAsibawatari_Normal_Vertical> vertical = std::make_unique<G5_StageAsibawatari_Normal_Vertical>();
			vertical->SetPositionX(posX);
			vertical->SetPositionZ(0);
			vertical->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(vertical));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 8:
	{
		if (gameTimer > 8.0f)
		{
			//! 通常
			std::unique_ptr<G5_StageAsibawatari_Normal> normal = std::make_unique<G5_StageAsibawatari_Normal>();
			normal->SetPositionX(posX);
			normal->SetPositionZ(-10);
			normal->SetMoveSpeed(5.0f);
			normal->SetScale(DirectX::XMFLOAT3(0.075f, 0.1f, 0.075f));

			StageManager::Instance().Register(std::move(normal));

			//! トラップ
			std::unique_ptr<G5_StageAsibawatari_Trap> trap = std::make_unique<G5_StageAsibawatari_Trap>();
			trap->SetPositionX(posX);
			trap->SetPositionZ(10);
			trap->SetMoveSpeed(5.0f);
			trap->SetScale(DirectX::XMFLOAT3(0.07f, 0.1f, 0.07f));

			StageManager::Instance().Register(std::move(trap));

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 9:
	{
		if (gameTimer > 4.0f)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 5; j++)
					//! 横長
				{
					std::unique_ptr<G5_StageAsibawatari_Normal_Horizontal> horizontal = std::make_unique<G5_StageAsibawatari_Normal_Horizontal>();
					horizontal->SetPositionX(posX + (25 * j));
					float num = 0;
					switch (j)
					{
					case 0: num = 12; break;
					case 1: num = 6; break;
					case 2: num = 10; break;
					case 3: num = 6; break;
					case 4: num = 12; break;
					default:
						break;
					}

					float posZ = i == 0 ? num : -num;
					horizontal->SetPositionZ(posZ);
					horizontal->SetMoveSpeed(5.0f);

					StageManager::Instance().Register(std::move(horizontal));
				}
			}

			gameTimer = 0.0f;
			stageStep++;
		}
	}
	break;
	case 10:
	{
		if (gameTimer > 25.0f)
		{
			//! ゴール
			std::unique_ptr<G5_StageAsibawatari_Goal> goal = std::make_unique<G5_StageAsibawatari_Goal>();
			goal->SetPositionX(posX);
			goal->SetPositionZ(0);
			goal->SetMoveSpeed(5.0f);

			StageManager::Instance().Register(std::move(goal));

			newStageFinish = true;
		}
	}
	break;
	default:
		break;
	}
}

// シーン切り替え処理
void G5_Asibawatari::SceneChange()
{
	//! プレイヤーが落ちた、またはゲームをクリアしたなら
	if(player->GetPosition().y < -10.0f || gameClear)
	{
		if (!setFade)
		{
			//! フェードを設定
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				1.0f, 0.5f);

			setFade = true;
		}
		else if (setFade && !fade->GetFade())
		{
			BgmManager::Instance().UnloadBgm("足場渡り");

			std::unique_ptr<SceneLoading> loadingScene;
			if(player->GetPosition().y < -10.0f)
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G5_Asibawatari_GameOver>());
			else
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G5_Asibawatari_Clear>());

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}