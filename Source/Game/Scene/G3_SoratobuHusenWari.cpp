#include "Graphics/Graphics.h"
#include "G3_SoratobuHusenWari.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/G3_StageSoratobuHusenWari.h"
#include "Game/Stage/StageMoveFloor.h"
#include "Game/Character/Item/Balloon_Plus.h"
#include "Game/Character/Item/Balloon_Minus.h"
#include "Game/Character/Item/ItemManager.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Cloud.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "G3_SoratobuHusenWari_Result.h"
#include "Audio/BgmManager.h"

//! ムービー中か
bool G3_SoratobuHusenWari::movieScene = false;

//! スコア
int G3_SoratobuHusenWari::score = 0;

float G3_SoratobuHusenWari::gameTimer = 0.0f;

// 初期化
void G3_SoratobuHusenWari::Initialize()
{
	gameTimer = 0.0f;

	//! スコア
	score = 0;

	//! スコアの画像
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
	for(int i = 0; i < 3; i++)
	{
		StageManager& stageManager = StageManager::Instance();
		std::unique_ptr<G3_StageSoratobuHusenWari> stageMain = std::make_unique<G3_StageSoratobuHusenWari>();
	
		float posZ = 0.0f;
		if (i == 0)
		{
			posZ = 50.0f;
		}
		else if(i == 1)
		{
			posZ = 160.0f;
		}
		else
		{
			posZ = 270.0f;
		}

		stageMain->SetPositionZ(posZ);

		stageManager.Register(std::move(stageMain));
	}

	// プレイヤー初期化
	player = std::make_unique<Player3_SoratobuHusenWari>();
	player->SetPosition(DirectX::XMFLOAT3(0, 22, 0));
	player->SetGravity(0.0f);

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
	cameraController->SetTarget(DirectX::XMFLOAT3(0, 21.0f, 4.8f));
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(3), 0, 0));
	cameraController->SetRange(21.0f);

	// 背景
	backGround = std::make_unique<Sprite>();

	fade = std::make_unique<Fade>();
	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		1.0f, 0.5f);

	actionExplanation = std::make_unique<Sprite>("Data/Sprite/3.SoratobuHusenWari/ActionExplanation.png");

	//! ポーズ画面
	pause = std::make_unique<Pause>();

	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("空飛ぶ風船割り", "Data/Audio/Bgm/10.Soratobu.wav");
	bgm.PlayBgm("空飛ぶ風船割り", 0.5f);
}

// 終了化
void G3_SoratobuHusenWari::Finalize()
{
	//! ステージを破棄する
	StageManager::Instance().Clear();

	//! 敵を全て破棄する
	EnemyManager::Instance().Clear();

	//! エフェクトを全て破棄する
	EffectManager::Instance().Cleanup();

	ItemManager::Instance().Clear();
}

// 更新処理
void G3_SoratobuHusenWari::Update(float elapsedTime)
{
	//! ポーズ画面の更新処理
	pause->Update(elapsedTime);

	if (pause->GetPauseOpacity() <= 0.0f)
	{
		skydomeAngle.y -= DirectX::XMConvertToRadians(0.5f) * elapsedTime;

		float bgmVolume = 1.0f;
		float bgmSpeed = 1.0f;
		
		if (gameTimer < 30.0f)
		{
			bgmVolume = 0.5f;
			bgmSpeed = 1.0f;
		}
		else if (gameTimer < 100.0f)
		{
			bgmVolume = 0.7f;
			bgmSpeed = 1.2f;
		}
		else if (gameTimer < 150.0f)
		{
			bgmVolume = 0.9f;
			bgmSpeed = 1.5f;
		}
		else
		{
			bgmVolume = 1.0f;
			bgmSpeed = 2.0f;
		}

		BgmManager::Instance().ChangeBgmStatus("空飛ぶ風船割り", bgmVolume, bgmSpeed);

		if ((player->GetPosition().x < -2.5f && player->GetPosition().y < 18.0f) && actionExplanationOpacity > 0.0f)
			actionExplanationOpacity -= 2 * elapsedTime;
		else if(actionExplanationOpacity < 1.0f)
			actionExplanationOpacity += 2 * elapsedTime;
		
		actionExplanationOpacity = std::clamp(actionExplanationOpacity, 0.3f, 1.0f);

		//! フェードの更新処理
		fade->Update(elapsedTime);

		//! ターゲットを設定
		cameraController->SetTarget(DirectX::XMFLOAT3(0, 21.0f, 4.8f));
		Camera::Instance().Update(elapsedTime);
		cameraController->Update(elapsedTime);

		// ステージ更新処理
		StageManager::Instance().Update(elapsedTime);

		// プレイヤー更新処理
		player->Update(elapsedTime);

		// エネミー更新処理
		EnemyManager::Instance().Update(elapsedTime);

		// アイテムの更新処理
		ItemManager::Instance().Update(elapsedTime);

		// エフェクト更新処理
		EffectManager::Instance().Update(elapsedTime);

		// 衝突攻撃の更新処理
		collisionAttackManager.Update(elapsedTime);

		//! バルーン生成処理
		NewBalloon(elapsedTime);

		//! 雲生成処理
		NewCloud(elapsedTime);

		//! スコア更新処理
		UpdateScore();

		//! シーン切り替え
		SceneChange();
	}
}

// 描画処理
void G3_SoratobuHusenWari::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 5.0f;
	lightPosition.z = CameraController::target.z + 20.0f;
	lightRange = 200.0f;

	shadowMapEyeOffset = { 0.0f, 55.0f, 5.5f };

	//! フォグ
	fogColor = { 0,0,0 };
	fogStart = 20.0f;
	fogEnd   = 55.0f;

	skydomePosition.y = -1.25f;
	skydomeScale = 0.07f;

	Graphics& graphics = Graphics::Instance();

	DrawingSettings(graphics);

	if (!setEnvironmentMap)
	{
		renderTarget->GetEnvironmentMap()->Load("Data/Environment/SF_Night.hdr");
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

				// アイテム描画処理
				ItemManager::Instance().Render(dc, shadowMapShader);
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

			// アイテム描画処理
			ItemManager::Instance().Render(dc, shader);

			// プレイヤー描画
			player->Render(dc, shader);

			// 衝突攻撃の描画
			collisionAttackManager.Render(dc, shader);

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

#ifdef _DEBUG

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		player->DrawDebugPrimitive();

		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().DrawDebugPrimitive();

		// アイテム
		ItemManager::Instance().DrawDebugPrimitive();

		// 衝突攻撃
		collisionAttackManager.DrawDebugPrimitive();
	}
#endif // _DEBUG

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

		//! プレイヤーのスプライト描画
		player->SpriteRender(dc);

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
			1, 1, 1, actionExplanationOpacity);

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

			//! スコアの位置
			ImGui::DragFloat2("ScorePos", &scoreTextPos.x);
			//! スコア
			ImGui::DragInt("Score", &score);

			collisionAttackManager.DrawDebugGUI();
			ItemManager::Instance().DrawDebugGUI();

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

// 風船生成処理
void G3_SoratobuHusenWari::NewBalloon(float elapsedTime)
{
	if (newBalloonWaitTime > 0.0f)
	{
		newBalloonWaitTime -= elapsedTime;
		return;
	}

	ItemManager& itemManager = ItemManager::Instance();
	int ItemCount = itemManager.GetItemCount();

	if (ItemCount < 10)
	{
		int itemRansu = rand() % 2 + 1;

		DirectX::XMFLOAT3 pos;
		pos.x = rand() % 11 * (rand() % 2 == 1 ? -1 : 1);
		pos.y = rand() % 8 + 17.0f;
		pos.z = 100.0f;

		switch (itemRansu)
		{
		case 1:
		{
			std::unique_ptr<Balloon_Plus> balloon_Plus = std::make_unique<Balloon_Plus>();

			balloon_Plus->SetPosition(pos);
			balloon_Plus->SetPositionResetY(pos.y);

			itemManager.Register(std::move(balloon_Plus));
		}
			break;
		case 2:
		{
			std::unique_ptr<Balloon_Minus> balloon_Minus = std::make_unique<Balloon_Minus>();

			balloon_Minus->SetPosition(pos);
			balloon_Minus->SetPositionResetY(pos.y);

			itemManager.Register(std::move(balloon_Minus));
		}
			break;
		default:
			break;
		}
		newBalloonWaitTime = 1.3f;

		if (gameTimer < 30.0f)
			newBalloonWaitTime = 1.3f;
		else if (gameTimer < 100.0f)
			newBalloonWaitTime = 1.2f;
		else if (gameTimer < 150.0f)
			newBalloonWaitTime = 1.0f;
		else
			newBalloonWaitTime = 0.7f;
	}
}

// 雲生成処理
void G3_SoratobuHusenWari::NewCloud(float elapsedTime)
{
	gameTimer += elapsedTime;

	if (newCloudWaitTime > 0.0f)
	{
		newCloudWaitTime -= elapsedTime;
		return;
	}

	int collisionAttackCount = collisionAttackManager.GetCollisionAttackCount();
	int maxCollisionAttackCount = 20;

	if (collisionAttackCount < maxCollisionAttackCount)
	{
		DirectX::XMFLOAT3 pos;
		pos.x = rand() % 11 * (rand() % 2 == 1 ? -1 : 1);
		pos.y = rand() % 8 + 18.5f;
		pos.z = 100.0f;

		//! 雲
		std::unique_ptr<CollisionAttack_Cloud> cloud = std::make_unique<CollisionAttack_Cloud>(&collisionAttackManager);
		cloud->SetPosition(pos);

		collisionAttackManager.Register(std::move(cloud));

		if (G3_SoratobuHusenWari::gameTimer < 30.0f)
			newCloudWaitTime = 5.0f;
		else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
			newCloudWaitTime = 3.0f;
		else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
			newCloudWaitTime = 1.5f;
		else
			newCloudWaitTime = 0.5f;
	}
}

// スコア更新処理
void G3_SoratobuHusenWari::UpdateScore()
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

// シーン変更処理
void G3_SoratobuHusenWari::SceneChange()
{
	if(player->GetHp() <= 0)
	{
		if (!setFade)
		{
			//! フェードを設定
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				1.0f, 0.5f);

			setFade = true;
		}
		else if(setFade && !fade->GetFade())
		{
			BgmManager::Instance().UnloadBgm("空飛ぶ風船割り");

			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G3_SoratobuHusenWari_Result>());

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}