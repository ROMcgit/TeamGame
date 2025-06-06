#include "Graphics/Graphics.h"
#include "G4_OssanTataki.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"

#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G4_StageOssanTataki.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Hole.h"
#include "Game/Character/Enemy/EnemyChara.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "G4_OssanTataki_Result.h"
#include "Audio/BgmManager.h"

//! スコア
int G4_OssanTataki::score = 0;

// 初期化
void G4_OssanTataki::Initialize()
{
	score = 0;

	movieScene = true;

	scoreText = std::make_unique<Text>();


	Graphics& graphics = Graphics::Instance();

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
	std::unique_ptr<G4_StageOssanTataki> stageMain = std::make_unique<G4_StageOssanTataki>();
	stageManager.Register(std::move(stageMain));

	// プレイヤー初期化
	player = std::make_unique<Player4_OssanTataki>();

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
	cameraController->SetTarget(DirectX::XMFLOAT3(10, 56, -44.0f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(49), 0, 0));
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

	actionExplanation = std::make_unique<Sprite>("Data/Sprite/4.OssanTataki/ActionExplanation.png");

	//! タイマー
	timer = std::make_unique<Timer>(true, 1, 30);

	//! ポーズ画面
	pause = std::make_unique<Pause>();

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("おっさん叩き", "Data/Audio/Bgm/11.Ossantataki.wav");
	bgm.PlayBgm("おっさん叩き", 0.3f);
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
	//! ポーズ画面
	pause->Update(elapsedTime);

	if(pause->GetPauseOpacity() <= 0.0f)
	{
		skydomeAngle.y -= DirectX::XMConvertToRadians(5) * elapsedTime;

		//! フェードの更新処理
		fade->Update(elapsedTime);

		//! タイマーの更新処理
		if (!movieScene)
			timer->Update(elapsedTime);

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
		if (!movieScene)
			player->Update(elapsedTime);

		// エネミー更新処理
		if (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 0))
			EnemyManager::Instance().Update(elapsedTime);

		// エフェクト更新処理
		EffectManager::Instance().Update(elapsedTime);

		// 敵生成処理
		NewEnemy(elapsedTime);

		// スコア更新処理
		UpdateScore();

		// シーン切り替え処理
		SceneChange();
	}
}

// 描画処理
void G4_OssanTataki::Render()
{
	lightColor = { 0.84f, 0.73f, 0.55f };
	lightPosition.x = 2.8f;
	lightPosition.y = 43.5f;
	lightPosition.z = -43.8f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 0.7f, 23.0f, 6.0f };

	skydomeScale = 0.117f;

	//! フォグ
	fogStart = 80.0f;
	fogEnd   = 380.0f;
	fogColor = { 1.0f, 0.436f, 0.0f };

	Graphics& graphics = Graphics::Instance();

	DrawingSettings(graphics);

	if (!setEnvironmentMap)
	{
		renderTarget->GetEnvironmentMap()->Load("Data/Environment/env_sky_002_sunset2.hdr");
		renderTarget->GetEnvironmentMap()->Set(15);

		setEnvironmentMap = true;
	}

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
				//EnemyManager::Instance().Render(dc, shadowMapShader);
				// プレイヤー描画
				//player->Render(dc, shadowMapShader);

				// 衝突攻撃の描画処理
				//collisionAttackManager.Render(dc, shadowMapShader);

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

			cameraController->RenderCameraTarget(dc, shader);

			// 衝突攻撃の描画処理
			collisionAttackManager.Render(dc, shader);

			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);
			shader->End(dc);
		}
	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

#ifdef _DEBUG

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		player->DrawDebugPrimitive();

		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().DrawDebugPrimitive();
	}

#endif // !_DEBUG

	// ラインレンダラ描画実行
	graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

	// デバッグレンダラ描画実行
	graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

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
		//! スコアの画像
		scoreText->RenderOku(dc, false, score, false,
			scoreTextPos.x, scoreTextPos.y);

		//! タイマー
		DirectX::XMFLOAT3 color;
		if (timer->GetTimeM_Int() > 0 || (timer->GetTimeM_Int() == 0 && timer->GetTimeS_Int() > 30))
			color = { 1, 1, 1 };
		else
			color = { 1, 0, 0 };

		timer->Render(dc, graphics, DirectX::XMFLOAT2(30.0f, 0.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f));

		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(actionExplanation->GetTextureWidth());
		float textureHeight = static_cast<float>(actionExplanation->GetTextureHeight());

		actionExplanation->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		//! フェードの描画処理
		fade->Render(dc, graphics);

		//! ポーズ画面
		pause->Render(dc, graphics);
	}

#ifdef _DEBUG

	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::ColorEdit3("BGColor", &backGroundColor.x);
			ImGui::InputFloat3("BGColorNum", &backGroundColor.x);

			ImGui::DragInt("Score", &score);

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
		}
		ImGui::End();
	}
#endif // !_DEBUG
}

// ムービー更新処理
void G4_OssanTataki::UpdateMovie(float elapsedTime)
{
	if (!movieScene) return;

	switch (movieStep)
	{
	case 0:

		movieTime += elapsedTime;

		if (movieTime > 6.0f)
		{
			movieScene = false;
		}

		break;
	}
}

// 敵生成処理
void G4_OssanTataki::NewEnemy(float elapsedTime)
{
	if (newEnemy) return;

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

#if 1
	for(int i = 0; i < 4; i++)
	{
		CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(i);

		// 位置
		DirectX::XMFLOAT3 pos = { 0, 0, 0 };
		pos.x = collisionAttack->GetPosition().x;
		pos.y = -4.5f;
		pos.z = collisionAttack->GetPosition().z;

		std::unique_ptr<EnemyChara> chara = std::make_unique<EnemyChara>();
		chara->SetPosition(pos);

		//! おっさんを登録
		EnemyManager::Instance().Register(std::move(chara));
	}
#endif

	newEnemy = true;
}

// スコア更新処理
void G4_OssanTataki::UpdateScore()
{
	//! スコアが最大値を超えないようにする
	if (score > scoreText->GetMaxOku())
	{
		score = scoreText->GetMaxOku();
	}
	else if (score < scoreText->GetMin())
	{
		score = scoreText->GetMin();
	}
}

// シーン切り替え処理
void G4_OssanTataki::SceneChange()
{
	if (timer->GetTimeM_Int() <= 0 && timer->GetTimeS_Int() <= 0)
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
			BgmManager::Instance().UnloadBgm("おっさん叩き");

			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G4_OssanTataki_Result>());

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}