#include "Graphics/Graphics.h"
#include "G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Character/Enemy/EnemyEye.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G0_StageOnigokko.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Scene/SceneTitle.h"
#include "Game/Scene/G0_Onigokko_Clear.h"
#include "Game/Scene/G0_Onigokko_GameOver.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneManager.h"
#include <algorithm>
#include "Audio/BgmManager.h"

bool G0_Onigokko::movieScene = false;

// 初期化
void G0_Onigokko::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// レンダーターゲット
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// シャドウマップ初期化
	shadowMap.Initialize();

/********************************************************************************/

	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	std::unique_ptr<G0_StageOnigokko> stageMain = std::make_unique<G0_StageOnigokko>();
	stageManager.Register(std::move(stageMain));

	// プレイヤー初期化
	player = std::make_unique<Player0_Onigokko>();
	player->SetPosition(DirectX::XMFLOAT3(0, 5.0f, 0));
	player->SetMovieTime(10.0f);
	player->SetMovieAnimation(0, true);

	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
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
	cameraController->SetRange(14);
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(10), 0, 0));

	// 背景
	backGround = std::make_unique<Sprite>();

	for(int i = 0; i < 3; i++)
	{
		float posX = 0;
		switch (i + 1)
		{
		case 1: posX = 0; break;
		case 2: posX = -5;  break;
		case 3: posX = 5;  break;
		default:
			break;
		}
		// 鬼
		bool s = i == 0 ? true : false;
		std::unique_ptr<EnemyOni> oni = std::make_unique<EnemyOni>(s);
		oni->SetPosition(DirectX::XMFLOAT3(posX, 5, 250));
		EnemyManager::Instance().Register(std::move(oni));
	}

	float posZ = 0;
	float posX = 0;
	for (int i = 0; i < 8; i++)
	{
		switch (i + 1)
		{
		case 1:
		case 2:
		{
			posZ = 0 * (i + 1 == 2 ? 1 : -1);
		}
		break;
		case 3:
		case 4:
		{
			posZ = rand() % 5 + 80 * (i + 1 == 4 ? 1 : -1);
		}
		break;
		case 5:
		case 6: 
		{
			posZ = rand() % 5 + 160 * (i + 1 == 6 ? 1 : -1);
		}
			break;
		case 7:
		case 8:
		{
			posZ = rand() % 5 + 240 * (i + 1 == 8 ? 1 : -1);
		}
		break;
		default:
			break;
		}


		for (int j = 0; j < 8; j++)
		{
			switch (j + 1)
			{
			case 1:
			case 2:
			{
				posX = 30 * (j + 1 == 2 ? 1 : -1);
			}
			break;
			case 3:
			case 4:
			{
				posX = rand() % 5 + 80 * (j + 1 == 4 ? 1 : -1);
			}
			break;
			case 5:
			case 6:
			{
				posX = rand() % 5 + 160 * (j + 1 == 6 ? 1 : -1);
			}
			break;
			case 7:
			case 8:
			{
				posX = rand() % 5 + 240 * (j + 1 == 8 ? 1 : -1);
			}
			break;
			default:
				break;
			}

			std::unique_ptr<EnemyEye> eye = std::make_unique<EnemyEye>();
			eye->SetPosition(DirectX::XMFLOAT3(posX, 7, posZ));
			EnemyManager::Instance().Register(std::move(eye));
		}
	}

	//! フェード
	fade = std::make_unique<Fade>();
	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);

	// タイマー
	timer = std::make_unique<Timer>(true, 2);

	// ムービーシーンにする
	movieScene = true;

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("おにごっこ", "Data/Audio/Bgm/7.Onigokko.wav");
	bgm.PlayBgm("おにごっこ", 1.0f);
}

// 終了化
void G0_Onigokko::Finalize()
{
	//! ステージを破棄する
	StageManager::Instance().Clear();

	//! 敵を全て破棄する
	EnemyManager::Instance().Clear();

	//! エフェクトを全て破棄する
	EffectManager::Instance().Cleanup();
}

// 更新処理
void G0_Onigokko::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	if (!movieScene || cameraMovieScene == CameraMovieScene::OniMove)
	{
		DirectX::XMFLOAT3 cameraTarget = player->GetPosition();
		cameraTarget.y += player->GetHeight() * 0.6f;

		if (cameraTarget.x < -280 || cameraTarget.x > 280)
		{
			cameraTarget.x = std::clamp(cameraTarget.x, -280.0f, 280.0f);
		}

		if (cameraTarget.z < -280 || cameraTarget.z > 280)
		{
			cameraTarget.z = std::clamp(cameraTarget.z, -280.0f, 280.0f);
		}

		// ターゲットを設定
		cameraController->SetTarget(cameraTarget);
		// カメラの範囲
		cameraController->SetRange(25.0f);
		// カメラの角度
		cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(40), 0, 0));
	}
	else
		cameraController->SetTarget(target);
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	fade->Update(elapsedTime);

	if(!movieScene)
	{
		// タイマーの更新処理
		timer->Update(elapsedTime);
	}

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	//! プレイヤーの位置制限
	PlayerPositionControll();
	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// カメラのムービー更新処理
	UpdateCameraMovie(elapsedTime);

	//! 敵を生成
	NewEnemy();
}

// 描画処理
void G0_Onigokko::Render()
{
	std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

	if (movieScene && cameraMovieScene != CameraMovieScene::OniMove)
		lightPosition = oni->GetPosition();
	else
		lightPosition = player->GetPosition();


	lightRange = 80.0f;

	//! フォグ
	fogStart = 20.0f;
	fogEnd = 100.0f;
	fogColor = { 0, 0, 0 };

	shadowMapEyeOffset = { 4.0f, 17.0f, 9.0f };

	Graphics& graphics = Graphics::Instance();

	DrawingSettings(graphics);

	//! シャドウマップ
	{
		// ポーズ画面じゃないなら
		//if (pause->GetPause_BurioHuntersOpacity() < 1.0f)
		{
			//! シャドウマップ開始
			shadowMap.Begin(rc);
			{
				Shader* shadowMapShader = graphics.GetShadowMapShader();
				shadowMapShader->Begin(dc, rc);

				//エネミー描画
				EnemyManager::Instance().Render(dc, shadowMapShader);
				// プレイヤー描画
				player->Render(dc, shadowMapShader);

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
			0.8f, 0, 0, 1);

		ID3D11DepthStencilState* depthEnabledState = graphics.GetDepthEnabledState();
		dc->OMSetDepthStencilState(depthEnabledState, 0);
	}

	// 3Dモデル描画
	{
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

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	{
		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

#ifndef _DEBUG

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		player->DrawDebugPrimitive();
	}
#endif // _DEBUG

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

	{
		if (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() == 0)
			timer->Render(dc, graphics, DirectX::XMFLOAT2(30, 0), DirectX::XMFLOAT4(1, 1, 0, 1));
		else
			timer->Render(dc, graphics, DirectX::XMFLOAT2(30, 0));
	
		fade->Render(dc, graphics);
	}

#ifndef _DEBUG

	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			int enemyCount = EnemyManager::Instance().GetEnemyCount();
			ImGui::InputInt("EnemyCount", &enemyCount);

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

			StageManager::Instance().DrawDebugGUI();

			//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-----------------------------------------------------------------------------------------------------//
			EnemyManager::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}
#endif // _DEBUG
}

// プレイヤーの位置制限
void G0_Onigokko::PlayerPositionControll()
{
	float setPosX = 295.0f;
	if (player->GetPosition().x < -setPosX || player->GetPosition().x > setPosX)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -setPosX, setPosX);
		player->SetPositionX(positoinX);
	}

	float setPosZ = 287.0f;
	if (player->GetPosition().z < -setPosZ || player->GetPosition().z > setPosZ)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -setPosZ, setPosZ);
		player->SetPositionZ(positoinZ);
	}
}

// カメラのムービー更新処理
void G0_Onigokko::UpdateCameraMovie(float elapsedTime)
{

	switch (cameraMovieScene)
	{
	case CameraMovieScene::OniEntry:
	{
		cameraMovieTime += elapsedTime;

		std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

		target    = oni->GetPosition();
		target.y += oni->GetHeight() * 0.6f;

		if (cameraMovieTime > 8.0f)
		{
			cameraMovieTime = 0;
			movieScene = false;
			cameraMovieScene = CameraMovieScene::OniMove;
		}
	}
	break;
	case CameraMovieScene::OniMove:
	{
		if (movieScene)
		{
			if (!movieFade)
			{
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					1.2f, 0.2f);

				movieFade = true;
			}

			movieWaitTimer -= elapsedTime;
			if (movieWaitTimer <= 0.0f)
				movieScene = false;
		}
		else
		{
			movieWaitTimer = 2;

			if (movieFade && !fade->GetFade())
			{
				if (player->GetHp() <= 0)
				{
					BgmManager::Instance().UnloadBgm("おにごっこ");

					std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G0_Onigokko_GameOver>());
					// シーンマネージャーにローディングシーンへの切り替えを指示
					SceneManager::Instance().ChangeScene(std::move(loadingScene));
				}

				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					1.0f, 0.0f,
					1.2f, 0.8f);

				movieFade = false;
			}
		}

		if (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() == 0)
		{
			movieScene = true;
			player->SetMovieTime(100);

			if (!nextOnideathFade)
			{
				fade->SetFadeUnlock();
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					1.0f, 0.2f);
				nextOnideathFade = true;
			}

			if(!fade->GetFade())
				cameraMovieScene = CameraMovieScene::OniDeath;
		}
	}
	break;
	case CameraMovieScene::OniDeath:
	{
		cameraMovieTime += elapsedTime;

		if (!oniDeathFade && !fade->GetFade())
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				1.0f, 0.0f,
				1.0f, 0.2f);

			oniDeathFade = true;
		}

		std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

		cameraController->SetRange(30);
		cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(10), 0, 0));
		target = oni->GetPosition();
		target.y = oni->GetHeight() * 0.8f;

		if (cameraMovieTime > 4.0f)
		{
			if (!gameFinishFade)
			{
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					1.0f, 0.2f);

				gameFinishFade = true;
			}
			else if (!fade->GetFade())
			{
				BgmManager::Instance().UnloadBgm("おにごっこ");

				std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G0_Onigokko_Clear>());
				// シーンマネージャーにローディングシーンへの切り替えを指示
				SceneManager::Instance().ChangeScene(std::move(loadingScene));
			}
		}
	}
	break;
	default:
		break;
	}
}

// 敵生成処理
void G0_Onigokko::NewEnemy()
{
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	if (timer->GetTimeM_Int() < 1 && enemyCount < 5 && !EnemyOni::tracking)
	{
		// 鬼
		std::unique_ptr<EnemyOni> oni = std::make_unique<EnemyOni>();
		oni->SetPosition(DirectX::XMFLOAT3(0, 0, 250));
		EnemyManager::Instance().Register(std::move(oni));
	}
}