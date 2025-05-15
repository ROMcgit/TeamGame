#include "Graphics/Graphics.h"
#include "G2_Sundome.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G2_StageSundome.h"
#include "Game/Stage/StageMoveFloor.h"

//! ムービー中か
bool G2_Sundome::movieScene = false;

//! スコア
int G2_Sundome::score[3] = { 0, 0, 0 };

// 初期化
void G2_Sundome::Initialize()
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
	std::unique_ptr<G2_StageSundome> stageMain = std::make_unique<G2_StageSundome>();
	stageManager.Register(std::move(stageMain));

	// プレイヤー初期化
	player = std::make_unique<Player2_Sundome>();
	player->SetPosition(DirectX::XMFLOAT3(215.0f, 70.0f, -2.0f));

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
	cameraController->SetTarget(DirectX::XMFLOAT3(-176.5, 61.5f, -2.8f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(45), 0, 0));
	cameraController->SetRange(42.5f);

	//! ムービーにする
	movieScene = true;

	// 背景
	backGround = std::make_unique<Sprite>();

	//! フェード
	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);
}

// 終了化
void G2_Sundome::Finalize()
{
	//! ステージを破棄する
	StageManager::Instance().Clear();

	//! 敵を全て破棄する
	EnemyManager::Instance().Clear();

	//! エフェクトを全て破棄する
	EffectManager::Instance().Cleanup();
}

// 更新処理
void G2_Sundome::Update(float elapsedTime)
{
	//! フェードの更新処理
	fade->Update(elapsedTime);

	//! ムービー更新処理
	UpdateMovie(elapsedTime);

	if(!movieScene || (movieScene && player->GetRound() >= 2))
	{
		// カメラコントローラー更新処理
		DirectX::XMFLOAT3 target = player->GetPosition();
		target.y = 63.75f;

		cameraController->SetTarget(target);
	}
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void G2_Sundome::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 15.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	//! シャドウマップ
	shadowMapEyeOffset = { -18.0f, 53.0f, 9.0f };

	//! フォグ
	fogStart = 80.0f;
	fogEnd   = 130.0f;

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
			backGroundColor.x, backGroundColor.y, backGroundColor.z, 1);

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
		player->SpriteRender(dc, graphics);

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
void G2_Sundome::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	switch (movieStep)
	{
	//! カメラの位置を変更する
	case 0:

		//! ムービー時間を計測
		movieTime += elapsedTime;

		if (movieTime > 2.0f)
		{
			// カメラの位置を変更する
			cameraController->SetTargetChange(
				DirectX::XMFLOAT3(
					Player2_Sundome::Instance().GetPosition().x - 10.0f, 
					CameraController::target.y,
					CameraController::target.z),
					5.0f);

			//! ムービー時間を0にする
			movieTime = 0.0f;

			movieStep++;
		}
		break;
	//! プレイヤーに近づける
	case 1:

		//! カメラのターゲットの変更が終わったら
		if (!CameraController::targetChange)
		{
			//! ムービー時間を計測
			movieTime += elapsedTime;

			//! ムービー時間が1より大きいなら
			if(movieTime > 1.0f)
			{
				DirectX::XMFLOAT3 pos = player->GetPosition();
				pos.y = 63.75f;

				//! カメラの位置を変更する
				cameraController->SetTargetChange(pos, 2.0f);

				//! カメラの範囲を変更する
				cameraController->SetRangeChange(29.0f, 2.0f);

				//! ムービー時間を0にする
				movieTime = 0.0f;

				movieStep++;
			}
		}
		break;
	//! ムービーを終わる
	case 2:

		if (!CameraController::targetChange)
		{
			//! ムービー時間を計測
			movieTime += elapsedTime;

			if (movieTime > 1.0f)
			{
				movieScene = false;
				movieStep++;
			}
		}
		break;
	//! 戻るムービー
	case 3:

		movieTime = 0.0f;
		movieStep++;

		break;
	//! ムービー待ち時間
	case 4:

		movieTime += elapsedTime;

		if (movieTime > 2.3f)
		{
			movieStep = 3;
			movieScene = false;
		}

		break;
	default:
		break;
	}
}