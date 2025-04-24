#include "Graphics/Graphics.h"
#include "G4_OssanTataki.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyManager1.h"
#include "Game/Character/Enemy/EnemyManager2.h"
#include "Game/Character/Enemy/EnemyManager3.h"

#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G4_StageOssanTataki.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Hole.h"
#include "Game/Character/Enemy/EnemyOssan.h"
#include "Game/Character/Enemy/EnemyOssan1.h"
#include "Game/Character/Enemy/EnemyOssan2.h"
#include "Game/Character/Enemy/EnemyOssan3.h"

bool G4_OssanTataki::isEnemy[4] = { false, false, false, false };

// 初期化
void G4_OssanTataki::Initialize()
{
	for (int i = 0; i < 4; i++)
	{
		isEnemy[i] = false;
	}

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
	std::unique_ptr<G4_StageOssanTataki> stageMain = std::make_unique<G4_StageOssanTataki>();
	stageManager.Register(std::move(stageMain));

	// プレイヤー初期化
	player = std::make_unique<Player4_OssanTataki>();

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
	cameraController->SetTarget(DirectX::XMFLOAT3(10, 56, -39.0f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(63), 0, 0));
	cameraController->SetRange(56.0f);

	for (int i = 0; i < 4; i++)
	{
		// 穴
		std::unique_ptr<CollisionAttack_Hole> hole = std::make_unique<CollisionAttack_Hole>(&collisionAttackManager);
		//! 位置
		DirectX::XMFLOAT3 pos = { 10.0f, 21.5f, -19.0f };

		float offsetX = 32.0f;
		float offsetZ = 17.5f;
		switch (i + 1)
		{
		case 1:
			pos.x -= offsetX;
			break;
		case 2:
			pos.x += offsetX;
			break;
		case 3:
			pos.z += offsetZ;
			break;
		case 4:
			pos.z -= offsetZ;
			break;
		default:
			break;
		}

		//! 位置を設定
		hole->SetPosition(pos);

		collisionAttackManager.Register(std::move(hole));
	}

	// 背景
	backGround = std::make_unique<Sprite>();

	//! フェード
	fade = std::make_unique<Fade>();
	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);
}

// 終了化
void G4_OssanTataki::Finalize()
{
	//! ステージを破棄する
	StageManager::Instance().Clear();

	//! 敵を全て破棄する
	EnemyManager::Instance().Clear();

	//! エフェクトを全て破棄する
	EffectManager::Instance().Cleanup();
}

// 更新処理
void G4_OssanTataki::Update(float elapsedTime)
{
	//! フェードの更新処理
	fade->Update(elapsedTime);

	//! ムービー更新処理
	UpdateMovie(elapsedTime);

	// 衝突攻撃の更新処理
	collisionAttackManager.Update(elapsedTime);

	// カメラコントローラー更新処理
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);
	EnemyManager1::Instance().Update(elapsedTime);
	EnemyManager2::Instance().Update(elapsedTime);
	EnemyManager3::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// 敵生成処理
	NewEnemy(elapsedTime);
}

// 描画処理
void G4_OssanTataki::Render()
{
	lightPosition.x = 2.8f;
	lightPosition.y = 43.5f;
	lightPosition.z = -43.8f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 0.7f, 23.0f, 6.0f };

	shadowMapBias = 0.015f;

	//! フォグ
	fogStart = 2000.0f;
	fogEnd = 2100.0f;

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
				EnemyManager1::Instance().Render(dc, shadowMapShader);
				EnemyManager2::Instance().Render(dc, shadowMapShader);
				EnemyManager3::Instance().Render(dc, shadowMapShader);
				// プレイヤー描画
				player->Render(dc, shadowMapShader);

				// 衝突攻撃の描画処理
				collisionAttackManager.Render(dc, shadowMapShader);

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
		player->Render(dc, shader);

		cameraController->RenderCameraTarget(dc, shader);

		// 衝突攻撃の描画処理
		collisionAttackManager.Render(dc, shader);

		//エネミー描画
		EnemyManager::Instance().Render(dc, shader);
		EnemyManager1::Instance().Render(dc, shader);
		EnemyManager2::Instance().Render(dc, shader);
		EnemyManager3::Instance().Render(dc, shader);
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
		EnemyManager1::Instance().DrawDebugPrimitive();
		EnemyManager2::Instance().DrawDebugPrimitive();
		EnemyManager3::Instance().DrawDebugPrimitive();

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
		Camera::Instance().SetPostEffectStatusOnce();
		//! スクリーンをポストエフェクトシェーダーで描画
		renderTarget->Render();
	}

	{
		//! フェードの描画処理
		fade->Render(dc, graphics);
	}

	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			collisionAttackManager.DrawDebugGUI();
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
			EnemyManager1::Instance().DrawDebugGUI();
			EnemyManager2::Instance().DrawDebugGUI();
			EnemyManager3::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}
}

// ムービー更新処理
void G4_OssanTataki::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	switch (movieStep)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}

// 敵生成処理
void G4_OssanTataki::NewEnemy(float elapsedTime)
{
	for (int i = 0; i < 4; i++)
	{
		if (newEnemyWaitTime[i] > 0.0f)
			newEnemyWaitTime[i] -= elapsedTime;
	}

	EnemyManager& enemyManager0 = EnemyManager::Instance();
	int enemyCount0 = enemyManager0.GetEnemyCount();

	if (enemyCount0 < 1 && newEnemyWaitTime[0] <= 0.0f)
	{
#if 1
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(0);

		// 位置
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan> ossan = std::make_unique<EnemyOssan>();
		ossan->SetPosition(pos);

		//! おっさんを登録
		EnemyManager::Instance().Register(std::move(ossan));

		newEnemyWaitTime[0] = 1.0f;
#endif
	}

	EnemyManager1& enemyManager1 = EnemyManager1::Instance();
	int enemyCount1 = enemyManager1.GetEnemyCount();

	if (enemyCount1 < 1 && newEnemyWaitTime[1] <= 0.0f)
	{
#if 1
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(1);

		// 位置
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan1> ossan = std::make_unique<EnemyOssan1>();
		ossan->SetPosition(pos);

		//! おっさんを登録
		EnemyManager1::Instance().Register(std::move(ossan));

		newEnemyWaitTime[1] = 1.0f;
#endif
	}

	EnemyManager2& enemyManager2 = EnemyManager2::Instance();
	int enemyCount2 = enemyManager2.GetEnemyCount();

	if (enemyCount2 < 1 && newEnemyWaitTime[2] <= 0.0f)
	{
#if 1
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(2);

		// 位置
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan2> ossan = std::make_unique<EnemyOssan2>();
		ossan->SetPosition(pos);

		//! おっさんを登録
		EnemyManager2::Instance().Register(std::move(ossan));

		newEnemyWaitTime[2] = 1.0f;
#endif
	}

	EnemyManager3& enemyManager3 = EnemyManager3::Instance();
	int enemyCount3 = enemyManager3.GetEnemyCount();

	if (enemyCount3 < 1 && newEnemyWaitTime[3] <= 0.0f)
	{
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(3);

		// 位置
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyOssan3> ossan = std::make_unique<EnemyOssan3>();
		ossan->SetPosition(pos);

		//! おっさんを登録
		EnemyManager3::Instance().Register(std::move(ossan));

		newEnemyWaitTime[3] = 1.0f;
	}
}