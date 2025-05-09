#include "Graphics/Graphics.h"
#include "G1_DarumasangaKoronda.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyDarumasangaKoronda.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G1_StageDarumasangaKoronda.h"
#include "Game/Stage/StageMoveFloor.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "G1_DarumasangaKoronda_Clear.h"
#include "G1_DarumasangaKoronda_GameOver.h"

//! ムービー中か
bool G1_DarumasangaKoronda::movieScene = false;

// 初期化
void G1_DarumasangaKoronda::Initialize()
{
	movieScene = true;

	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// レンダーターゲット
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// シャドウマップ初期化
	shadowMap.Initialize();

/********************************************************************************/

	float posZ = 54.0f;
	// ステージ初期化
	for(int i = 0; i < 5; i ++)
	{
		StageManager& stageManager = StageManager::Instance();
		std::unique_ptr<G1_StageDarumasangaKoronda> stageMain = std::make_unique<G1_StageDarumasangaKoronda>();
		
		float posZ = 54.0f + (i * 110.0f);

		stageMain->SetPosition(DirectX::XMFLOAT3(0, 110.0f, posZ));
		stageManager.Register(std::move(stageMain));
	}

	// プレイヤー初期化
	player = std::make_unique<Player1_DarumasangaKoronda>();
	player->SetPosition(DirectX::XMFLOAT3(0, 113.0f, 0));

	//! 敵
	std::unique_ptr<EnemyDarumasangaKoronda> oni = std::make_unique<EnemyDarumasangaKoronda>();
	oni->SetPosition(DirectX::XMFLOAT3(0.0f, 113.227f, 538.45f));
	//! 鬼を登録
	EnemyManager::Instance().Register(std::move(oni));

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
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(26.5f), 0, 0));
	cameraController->SetRange(20.0f);

	// 背景
	backGround = std::make_unique<Sprite>();

	//! フェード
	fade = std::make_unique<Fade>();
	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);

	//! タイマー
	timer = std::make_unique<Timer>(true, 3, 0);
}

// 終了化
void G1_DarumasangaKoronda::Finalize()
{
	//! ステージを破棄する
	StageManager::Instance().Clear();

	//! 敵を全て破棄する
	EnemyManager::Instance().Clear();

	//! エフェクトを全て破棄する
	EffectManager::Instance().Cleanup();
}

// 更新処理
void G1_DarumasangaKoronda::Update(float elapsedTime)
{
	//! フェードを更新処理
	fade->Update(elapsedTime);

	//! タイマーの更新処理
	if (!movieScene)
		timer->Update(elapsedTime);

	//! ムービー更新処理
	UpdateMovie(elapsedTime);

	// カメラコントローラー更新処理
	if (!movieScene || (movieStep == 1 || movieStep == 2))
	{
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.x = 0;
		target.y += player->GetHeight() * 0.5f;
		target.z -= 2.0f;
		if (target.z < 0.8f || target.z > 540.0f)
		{
			target.z = std::clamp(target.z, 0.8f, 540.0f);
		}
		cameraController->SetTarget(target);
		cameraController->SetRange(20.0f);
	}
	else
	{
		std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

		DirectX::XMFLOAT3 target = oni->GetPosition();
		target.x = 0;
		target.y += oni->GetHeight() * 0.5f;
		target.z -= 2.0f;
		cameraController->SetTarget(target);
		cameraController->SetRange(15.0f);
	}
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	PlayerPositionControll();
	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// シーン切り替え処理
	SceneChange();
}

// 描画処理
void G1_DarumasangaKoronda::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 150.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 4.0f, 17.0f, 9.0f };

	vignetteOpacity = 0.8f;

	//! フォグ
	fogStart = 50.0f;
	fogEnd   = 100.0f;

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
				std::unique_ptr<Enemy>& oni = EnemyManager::Instance().GetEnemy(0);

				if(oni->GetOpacity() > 0.5f)
					EnemyManager::Instance().Render(dc, shadowMapShader);
				// プレイヤー描画
				if (!movieScene)
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

		// プレイヤー描画
		if(!movieScene)
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

	{
		//! タイマー
		DirectX::XMFLOAT3 color;
		if (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 30))
			color = { 1, 1, 1 };
		else
			color = { 1, 0, 0 };

		timer->Render(dc, graphics, DirectX::XMFLOAT2(30.0f, 0.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f));

		//! フェードの描画処理
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

// ムービー更新処理
void G1_DarumasangaKoronda::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	player->SetMovieTime(0.1f);

	switch (movieStep)
	{
	case 0:

		movieTime += elapsedTime;

		if (movieTime > 3.5f)
		{
			movieScene = false;
			movieTime = 0.0f;
			movieStep++;
		}

		break;
	//! 敵死亡
	case 1:

		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			1.0f, 0.2f);

		movieStep++;

		break;
	//! フェードを終わる
	case 2:

		if (!fade->GetFade())
		{
			movieTime += elapsedTime;

			if (movieTime > 1.0f)
			{
				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					1.0f, 0.0f,
					1.0f, 0.2f);

				movieStep++;
			}
		}
		break;
	case 3:

		if (!fade->GetFade())
		{
			movieTime += elapsedTime;

			if (movieTime > 3.5f)
			{
				sceneChange = true;
			}
		}

		break;
	default:
		break;
	}
}

// シーン切り替え処理
void G1_DarumasangaKoronda::SceneChange()
{
	if (!sceneChange && (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 0))) return;

	if (!setFade)
	{
		//! フェードを設定
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			1.0f, 0.5f);

		setFade = true;
	}
	else if (!fade->GetFade())
	{
		std::unique_ptr<SceneLoading> loadingScene;
		if((timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 0)))
			loadingScene = std::make_unique<SceneLoading>(std::make_unique<G1_DarumasangaKoronda_Clear>());
		else
			loadingScene = std::make_unique<SceneLoading>(std::make_unique<G1_DarumasangaKoronda_GameOver>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// プレイヤーの位置制限
void G1_DarumasangaKoronda::PlayerPositionControll()
{
	float setPosX = 16.3f;
	if (player->GetPosition().x < -setPosX || player->GetPosition().x > setPosX)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -setPosX, setPosX);
		player->SetPositionX(positoinX);
	}

	float setPosZ = 287.0f;
	if (player->GetPosition().z < -2.0f || player->GetPosition().z > 545.0f)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -2.0f, 545.0f);
		player->SetPositionZ(positoinZ);
	}
}