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
	cameraController->SetRange(60);
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
	timer = std::make_unique<Timer>(true, 3);

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
	// タイマーの更新処理
	timer->Update(elapsedTime);

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
	Graphics& graphics = Graphics::Instance();

	DrawingSettings(graphics);

	if(EnemyManager::Instance().GetEnemyCount() > 0)
	{
		std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

		if (movieScene)
			lightPosition = oni->GetPosition();
		else
			lightPosition = player->GetPosition();
	}

	lightRange = 20.0f;

	//! フォグ
	fogStart = 12.0f;
	fogEnd   = 100.0f;
	fogColor = { 0, 0, 0 };

	//! レンダーターゲット
	{
		//! レンダーターゲットに描画開始
		renderTarget->Begin();
	}

	//! 2Dスプライト
	{
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
	}

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetDefaltLitShader();
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

	//! シェーダーを出す
	{
		//! レンダーターゲットへ描画終了
		renderTarget->End();
		//! スクリーンをポストエフェクトシェーダーで描画
		Camera::Instance().CreatePostEffect();
		Camera::Instance().SetPostEffectStatusOnce(
			1.0f, 0.8f,
			DirectX::XMFLOAT3(1.2f, 1.3f, 1.35f), 0);
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
	if (movieScene)
	{
		switch (cameraMovieScene)
		{
		case CameraMovieScene::OniEntry:
		{
			cameraMovieTime += elapsedTime;

			std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

			target = oni->GetPosition();
			target.y = oni->GetHeight() * 2.0f;

			if (cameraMovieTime > 10.0f)
			{
				cameraMovieTime  = 0;
				movieScene       = false;
				cameraMovieScene = CameraMovieScene::OniDeath;
			}
		}
		break;
		case CameraMovieScene::OniDeath:
		{
			cameraMovieTime += elapsedTime;

			if (cameraMovieTime > 5.0f)
			{
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					0.0f, 1.0f,
					1.0f, 5.0f);
			}
		}
		break;
		default:
			break;
		}
	}
}