#include "Graphics/Graphics.h"
#include "G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
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

	//カメラコントローラー初期化
	cameraController = std::make_unique <CameraController>();
	cameraController->SetRange(30);
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(10), 0, 0));

	// 背景
	backGround = std::make_unique<Sprite>();

	// 鬼
	std::unique_ptr<EnemyOni> oni = std::make_unique<EnemyOni>();
	oni->SetPosition(DirectX::XMFLOAT3(0, 5, 250));
	EnemyManager::Instance().Register(std::move(oni));

	fade = std::make_unique<Fade>();
	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);

	// タイマー
	timer = std::make_unique<Timer>(true, 2);

	// ムービーシーンにする
	movieScene = true;
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
	if (!movieScene)
	{
		target = player->GetPosition();
		target.y += 0.5f;
		cameraController->SetRange(110);
		cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(40), 0, 0));
	}
	
	cameraController->SetTarget(target);
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	fade->Update(elapsedTime);

	if(!movieScene)
	{
		if(EnemyManager::Instance().GetEnemyCount() < 50)
		{
			// 鬼
			std::unique_ptr<EnemyOni> oni = std::make_unique<EnemyOni>();
			{
				oni->SetPosition(DirectX::XMFLOAT3(100, 5, 200));
				EnemyManager::Instance().Register(std::move(oni));
			}
		}

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
}

// 描画処理
void G0_Onigokko::Render()
{
	std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

	if (movieScene && cameraMovieScene != CameraMovieScene::OniMove)
		lightPosition = oni->GetPosition();
	else
		lightPosition = player->GetPosition();


	lightRange = 30.0f;

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
			0, 0, screenWidth, screenHeight * 0.8f,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		ID3D11DepthStencilState* depthEnabledState = graphics.GetDepthEnabledState();
		dc->OMSetDepthStencilState(depthEnabledState, 0);
	}

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetDefaultLitShader();
		shader->Begin(dc, rc);
		// ステージ描画
		StageManager::Instance().Render(dc, shader);

		// カメラの位置を描画
		CameraController::Instance().RenderCameraTarget(dc, shader);

		// プレイヤー描画
		player->Render(dc, shader);

		//エネミー描画
		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);

	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
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
#endif // _DEBUG
}

// プレイヤーの位置制限
void G0_Onigokko::PlayerPositionControll()
{
	if (player->GetPosition().x < -445.0f || player->GetPosition().x > 445.0f)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -445.0f, 445.0f);
		player->SetPositionX(positoinX);
	}

	if (player->GetPosition().z < -445.0f || player->GetPosition().z > 445.0f)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -445.0f, 445.0f);
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

		target = oni->GetPosition();
		target.y = oni->GetHeight() * 0.8f;

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
					std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());
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
				std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());
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