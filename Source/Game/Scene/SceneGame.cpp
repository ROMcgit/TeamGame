#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Game/Scene/SceneLoading.h"
#include "SceneManager.h"
#include "Game/Scene/SceneGameOver.h"
#include "Game/Scene/SceneGameClear.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemySika.h"

#include "Game/Character/Item/ItemManager.h"
#include "Game/Character/Item/ImportantItemManager.h"
#include "Game/Character/Item/Apple.h"
#include "Game/Character/Item/Banana.h"

#include "Game/Character/Installation/InstallationManager.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/StageMain.h"

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	// プレイヤー初期化
	player = std::make_unique<Player>();

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

	//! カメラコントローラー初期化
	cameraController = std::make_unique <CameraController>();
	cameraController->SetAngle(DirectX::XMFLOAT3(
		DirectX::XMConvertToRadians(18),
		0, 0)
	);
	cameraController->SetRange(15.0f);

	//! 空
	backGround = std::make_unique<Sprite>("Data/Sprite/背景/空.png");

	fade = std::make_unique<Fade>();
}

// 終了化
void SceneGame::Finalize()
{
	// ステージ終了化
	StageManager::Instance().Clear();

	// エネミー終了化
	EnemyManager::Instance().Clear();

	// アイテム終了化
	ItemManager::Instance().Clear();

	// 設置物終了化
	InstallationManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// 生成処理
	Newestablishment(elapsedTime);

	// バナナ生成
	NewBanana(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	if(!enemyStop)
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// アイテム更新処理
	ItemManager::Instance().Update(elapsedTime);
	
	// 重要アイテム更新処理
	ImportantItemManager::Instance().Update(elapsedTime);

	// 設置物更新処理
	InstallationManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// フェードのアップデート
	fade->Update(elapsedTime);

	// ゲームクリア
	if (player->GetHealth() > 0 && player->GetBananaNum() == 7)
	{
		if (!setFade)
		{
			fade->SetFade(DirectX::XMFLOAT3(1, 1, 1),
				0.0f, 1.0f,
				3.5f);

			setFade = true;
		}

		if (!fade->GetFade())
		{
			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameClear>());

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}

	// ゲームオーバー
	if (player->GetHealth() <= 0)
	{
		if (!setFade)
		{
			fade->SetFade(DirectX::XMFLOAT3(1, 0, 0),
				0.0f, 1.0f,
				3.5f);

			setFade = true;
		}

		if (!fade->GetFade())
		{
			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameOver>());

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	float textureWidth = static_cast<float>(backGround->GetTextureWidth());
	float textureHeight = static_cast<float>(backGround->GetTextureHeight());

	//! 背景
	backGround->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// ステージ描画
		//stage->Render(dc, shader);
		StageManager::Instance().Render(dc, shader);

		// プレイヤー描画
		player->Render(dc, shader);

		//エネミー描画
		EnemyManager::Instance().Render(dc,shader);
		
		// アイテム描画処理
		ItemManager::Instance().Render(dc, shader);

		// 重要アイテム描画処理
		ImportantItemManager::Instance().Render(dc, shader);

		// 設置物描画処理
		InstallationManager::Instance().Render(dc, shader);
		
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

		// アイテムデブッグプリミティブ描画
		ItemManager::Instance().DrawDebugPrimitive();

		// 重要アイテムデバッグプリミティブ描画
		ImportantItemManager::Instance().DrawDebugPrimitive();

		// 設置物デバッグプリミティブ描画
		InstallationManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		player->SpriteRender(dc);
		EnemyManager::Instance().SpriteRender(dc);
		EnemyManager::Instance().RenderEnemyGauge(dc, rc.view, rc.projection);

		fade->Render(dc);
	}

	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::Checkbox("enemyStop", &enemyStop);

			EnemyManager& enemyManager = EnemyManager::Instance();
			ItemManager& itemManager = ItemManager::Instance();
			InstallationManager& installationManager = InstallationManager::Instance();

			int enemyCount        = enemyManager.GetEnemyCount();               // 敵の数
			int itemCount         = itemManager.GetItemCount();                 // アイテムの数
			int installationCount = installationManager.GetInstallationCount(); // 設置物の数

			/*! 敵の数 */
			ImGui::InputInt("EnemyCount", &enemyCount);
			ImGui::InputFloat("NewEnemyTimer", &newEnemyTimer);

//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
//-----------------------------------------------------------------------------------------------------//

			/*! アイテムの数 */
			ImGui::InputInt("ItemCount", &itemCount);
			ImGui::InputFloat("NewItemTimer", &newEnemyTimer);

//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
//-----------------------------------------------------------------------------------------------------//

			/*! 設置物の数 */
			ImGui::InputInt("InstallationCount", &installationCount);
			ImGui::InputFloat("InstallationTimer", &newInstallationTimer);

//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
//-----------------------------------------------------------------------------------------------------//

			// プレイヤーデバッグ描画
			player->DrawDebugGUI();
//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
//-----------------------------------------------------------------------------------------------------//
			camera.DrawDebugGUI();
//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
//-----------------------------------------------------------------------------------------------------//
			cameraController->DrawDebugGUI();

			ItemManager::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}
}

// 生成処理
void SceneGame::Newestablishment(float elapsedTime)
{
	EnemyManager& enemyManager               = EnemyManager::Instance();
	ItemManager&  itemManager                = ItemManager::Instance();
	InstallationManager& installationManager = InstallationManager::Instance();

	int enemyCount         = enemyManager.GetEnemyCount();               // 敵の数
	int itemCount          = itemManager.GetItemCount();                 // アイテムの数
	int installationCount  = installationManager.GetInstallationCount(); // 設置物の数

	if (player->GetBananaNum() < 1)
		enemyMaxCount = 10;
	else if(player->GetBananaNum() >= 1 && player->GetBananaNum() <= 4)
		enemyMaxCount = 15;
	else if((player->GetBananaNum() >= 5 && player->GetBananaNum() <= 7))
		enemyMaxCount = 20;

	if (player->GetPosition().y > 60)
	{
		for (int i = 0; i < enemyCount; i++)
		{
			std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);
			enemy->Destroy();
		}

		for (int i = 0; i < itemCount; i++)
		{
			std::unique_ptr<Item>& item = itemManager.GetItem(i);
			item->Destroy();
		}

		for (int i = 0; i < installationCount; i++)
		{
		}
	}

/***************************************************************************************************/

	if (player->GetIsGround())
	{
		float angleY = player->GetAngle().y;

		// 生成する範囲の角度をランダムに決める
		float randomAngle = angleY + (rand() % 360 - 180) * (3.14159f / 180.0f); // -180度から+180度までランダム

		float distance = rand() % 20 + 20; // プレイヤーからの距離もランダム

		int posX = player->GetPosition().x + distance * cos(randomAngle); // cosでX座標を計算
		int posZ = player->GetPosition().z + distance * sin(randomAngle); // sinでZ座標を計算

		if (enemyCount < enemyMaxCount && newEnemyTimer >  newEnemyMaxTimer)
		{
			posX = player->GetPosition().x + distance * cos(randomAngle);
			posZ = player->GetPosition().z + distance * sin(randomAngle);

			//! 位置制限(X)
			if (posX >= 990)
				posX = 950;
			else if (posX <= -990)
				posX = -950;

			//! 位置制限(Z)
			if (posZ >= 990)
				posZ = 950;
			else if (posZ <= -990)
				posZ = -950;

			std::unique_ptr<EnemySika> sika = std::make_unique<EnemySika>();
			sika->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
			enemyManager.Register(std::move(sika));

			newEnemyTimer = 0.0f;
		}

		if (itemCount < 5 && newItemTimer > newItemMaxTimer)
		{
			posX = player->GetPosition().x + distance * cos(randomAngle);
			posZ = player->GetPosition().z + distance * sin(randomAngle);

			//! 位置制限(X)
			if (posX >= 990)
				posX = 950;
			else if (posX <= -990)
				posX = -950;

			//! 位置制限(Z)
			if (posZ >= 990)
				posZ = 950;
			else if (posZ <= -990)
				posZ = -950;

			std::unique_ptr<Apple> apple = std::make_unique<Apple>();
			apple->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
			itemManager.Register(std::move(apple));

			newItemTimer = 0.0f;
		}

		if (installationCount < 1 && newInstallationTimer > newInstallationMaxTimer)
		{
			posX = player->GetPosition().x + distance * cos(randomAngle);
			posZ = player->GetPosition().z + distance * sin(randomAngle);

			newInstallationTimer = 0.0f;
		}

		newEnemyTimer += elapsedTime;
		newItemTimer += elapsedTime;
		newInstallationTimer += elapsedTime;
	}
}

// バナナ生成
void SceneGame::NewBanana(float elapsedTime)
{
	Player& player = Player::Instance();

	ImportantItemManager& importantItemManager = ImportantItemManager::Instance();
	int importantItemCount = importantItemManager.GetImportantItemCount();

	// バナナ生成までの時間
	if (importantItemCount == 0)
		newBananaWaitTimer -= elapsedTime;

	if (newBananaWaitTimer < 0.0f)
	{
		for (int i = 0; i < 7; i++)
		{
			// バナナを生成
			if (i == player.GetBananaNum() && !newBanana[i])
			{
				std::unique_ptr<Banana> banana = std::make_unique<Banana>();
				importantItemManager.Register(std::move(banana));
				newBananaWaitTimer = 1.0f;
				newBanana[i] = true;
			}
		}
	}
}