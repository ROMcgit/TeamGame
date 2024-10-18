#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Game/Scene/SceneLoading.h"
#include "SceneManager.h"
#include "Game/Scene/SceneGameOver.h"
#include "Game/Scene/SceneGameClear.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemySika.h"
#include "Game/Character/Enemy/SikaTentyo.h"

#include "Game/Character/Item/ItemManager.h"
#include "Game/Character/Item/ImportantItemManager.h"
#include "Game/Character/Item/Apple.h"
#include "Game/Character/Item/Melon.h"
#include "Game/Character/Item/Peach.h"
#include "Game/Character/Item/Mikan.h"
#include "Game/Character/Item/Banana.h"

#include "Game/Character/Installation/InstallationManager.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/StageMain.h"
#include "SceneTitle.h"

#include "Audio/BgmManager.h"
#include "Audio/SoundEffectManager.h"

// 初期化
void SceneGame::Initialize()
{
	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("バトル", "Data/Audio/bgm/battle.wav");
	bgm.PlayBgm("バトル", 0.55f);
	bgm.LoadBgm("ボス", "Data/Audio/bgm/boss.wav");
	bgm.LoadBgm("無敵", "Data/Audio/bgm/無敵.wav");

	SoundEffectManager::Instance().LoadSoundEffect("警告音", "Data/Audio/警告音.wav");
	SoundEffectManager::Instance().LoadSoundEffect("着地", "Data/Audio/着地.wav");
	SoundEffectManager::Instance().LoadSoundEffect("鳴き声", "Data/Audio/鳴き声.wav");

	// 衝撃波エフェクト
	shockWaveEffect = std::make_unique<Effect>("Data/Effect/ShockWave.efc");

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
	cameraController->SetRange(18.0f);

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
	if (player->GetInvincibleState())
	{
		if (!mutekiBgmPlay)
		{
			BgmManager::Instance().PlayBgm("無敵", 0.4f);
			mutekiBgmPlay = true;
		}

		BgmManager::Instance().ChangeBgmStatus("バトル", 0);
		BgmManager::Instance().ChangeBgmStatus("ボス", 0);
	}
	else
	{
		BgmManager::Instance().StopBgm("無敵");
		mutekiBgmPlay = false;

		BgmManager::Instance().ChangeBgmStatus("バトル", 0.55f);
		BgmManager::Instance().ChangeBgmStatus("ボス", 1.0f);
	}

	// 生成処理
	Newestablishment(elapsedTime);

	// バナナ生成
	NewBanana(elapsedTime);

	// ムービー
	UpdateMovie(elapsedTime);

	// カメラコントローラー更新処理
	if (!cameraController->GetCameraMovie() && !setMovie)
	{
		target = player->GetPosition();
		target.y += 0.5f;
	}
	else if (!cameraController->GetCameraMovie() && setMovie)
	{
		target = player->GetPosition();
		target.y += 0.4f;

		cameraController->SetRange(16.0f);

		cameraController->SetAngle(DirectX::XMFLOAT3(
			DirectX::XMConvertToRadians(12),
			0, 0)
		);

		if (!bgmPlay)
		{
			BgmManager::Instance().PlayBgm("ボス");
			bgmPlay = true;
		}
	}

	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

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

	SceneTitle& scene = SceneTitle::Instance();
	// ゲームクリア
	if (player->GetHealth() > 0 && scene.gameClear)
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
			BgmManager::Instance().UnloadBgm("バトル");
			BgmManager::Instance().UnloadBgm("ボス");

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
			BgmManager::Instance().UnloadBgm("バトル");
			BgmManager::Instance().UnloadBgm("ボス");

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
	FLOAT color[] = { 1.0f, 0.0f, 0.0f, 1.0f };	// RGBA(0.0～1.0)
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
		1, 1, 1, backGroundOpacity);

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// ステージ描画
		StageManager::Instance().Render(dc, shader);

		SceneTitle& scene = SceneTitle::Instance();
		if(!scene.gameClear)
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

#ifdef _DEBUG

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
#endif

	// 2Dスプライト描画
	{
		player->SpriteRender(dc);
		EnemyManager::Instance().SpriteRender(dc);
		EnemyManager::Instance().RenderEnemyGauge(dc, rc.view, rc.projection);

		fade->Render(dc);
	}

#ifdef _DEBUG
	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::Checkbox("enemyStop", &enemyStop);

			EnemyManager& enemyManager = EnemyManager::Instance();
			ItemManager& itemManager = ItemManager::Instance();
			InstallationManager& installationManager = InstallationManager::Instance();

			int enemyCount = enemyManager.GetEnemyCount();               // 敵の数
			int itemCount = itemManager.GetItemCount();                 // アイテムの数
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

			itemManager.DrawDebugGUI();

			enemyManager.DrawDebugGUI();
		}
		ImGui::End();
	}
#endif // !_DEBUG
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
	else if (player->GetBananaNum() >= 1 && player->GetBananaNum() <= 3)
		enemyMaxCount = 15;
	else if ((player->GetBananaNum() >= 4 && player->GetBananaNum() < 6))
		enemyMaxCount = 20;
	else if (player->GetBananaNum() >= 6)
		enemyMaxCount = 8;

	if (player->GetPosition().y > 60)
	{
		if (player->GetBananaNum() < 6)
		{
			for (int i = 0; i < enemyCount; i++)
			{
				std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);
				enemy->Destroy();
			}
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

	if (player->GetIsGround() && !cameraController->GetCameraMovie())
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
		else if (player->GetBananaNum() >= 6 && !enemyDelete)
		{
			for (int i = 0; i < enemyCount; i++)
			{
				std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);
				enemy->Destroy();
			}

			enemyDelete = true;
		}

		if (!cameraController->GetCameraMovie() && itemCount < 20 && newItemTimer > newItemMaxTimer)
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

			int ItemRand = rand() % 20 + 1;
			switch (ItemRand)
			{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			{
				std::unique_ptr<Apple> apple = std::make_unique<Apple>();
				apple->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(apple));
			}
			break;
			case 9:
			case 10:
			{
				std::unique_ptr<Melon> melon = std::make_unique<Melon>();
				melon->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(melon));
			}
			break;
			case 11:
			case 12:
			{
				std::unique_ptr<Peach> peach = std::make_unique<Peach>();
				peach->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(peach));
			}
			break;
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			{
				std::unique_ptr<Mikan> mikan = std::make_unique<Mikan>();
				mikan->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(mikan));
			}
			default:
				break;
			}

			newItemTimer = 0.0f;
		}

		if (!cameraController->GetCameraMovie() && installationCount < 1 && newInstallationTimer > newInstallationMaxTimer)
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
		for (int i = 0; i < 6; i++)
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

// ムービー
void SceneGame::UpdateMovie(float elapsedTime)
{
	SceneTitle& scene = SceneTitle::Instance();
	if (scene.gameClear)
	{
		BgmManager::Instance().StopBgm("ボス");

		if (!setGameClearMovie)
		{
			doCameraMovieTimer = 0.0f;

			EnemyManager& enemyManager = EnemyManager::Instance();
			std::unique_ptr<Enemy>& sikaTentyo = enemyManager.GetEnemy(0);

			DirectX::XMFLOAT3 pos = sikaTentyo->GetPosition();
			pos.y += 5;

			cameraTarget = { pos };

			//! カメラの角度
			cameraAngle = { DirectX::XMFLOAT3(
				DirectX::XMConvertToRadians(-6),
				DirectX::XMConvertToRadians(0),
				DirectX::XMConvertToRadians(0)) };

			cameraRange = 20.0f;

			player->SetMovieTime(20.0f);
			cameraController->SetCameraMovieTime(20.0f);

			setGameClearMovie = true;
		}

		if (cameraController->GetCameraMovie())
		{
			cameraController->SetAngle(cameraAngle);
			target = cameraTarget;
			cameraController->SetRange(cameraRange);
		}
	}
	else if (!setMovie && player->GetBananaNum() >= 6)
	{
		BgmManager::Instance().StopBgm("バトル");

		player->SetMovieTime(12.5f);
		cameraController->SetCameraMovieTime(12.5f);

		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			3.0f);

		setMovie = true;
	}

	//! カメラがムービー中なら
	if (cameraController->GetCameraMovie())
	{
		doCameraMovieTimer += elapsedTime;

		if (doCameraMovieTimer < 2.5f)
		{
			target = player->GetPosition();
			target.y += 0.5f;
		}
		else
		{
			if(backGroundOpacity > 0.4f)
			backGroundOpacity -= 0.13f * elapsedTime;

			if (!setMovieFade)
			{
				SoundEffectManager::Instance().PlaySoundEffect("警告音");

				player->SetPosition(DirectX::XMFLOAT3(0, 1.3f, -100));

				//! カメラのターゲット
				cameraTarget = { 0, 6, 0 };

				//! カメラの範囲
				cameraRange = 10.0f;

				//! カメラの角度
				cameraAngle = { DirectX::XMFLOAT3(
					DirectX::XMConvertToRadians(-6),
					DirectX::XMConvertToRadians(0),
					DirectX::XMConvertToRadians(0))
				};

				// プレイヤーの角度を設定
				player->SetAngle(DirectX::XMFLOAT3(
					DirectX::XMConvertToRadians(0),
					DirectX::XMConvertToRadians(0),
					DirectX::XMConvertToRadians(0)));

				fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
					1.0f, 0.0f,
					3.0f);

				setMovieFade = true;
			}

			if (doCameraMovieTimer > 2.5f && !newSikaTentyo)
			{
				std::unique_ptr<SikaTentyo> sikaTenstyo = std::make_unique<SikaTentyo>();
				sikaTenstyo->SetPosition(DirectX::XMFLOAT3(0, 120, 10));
				EnemyManager::Instance().Register(std::move(sikaTenstyo));

				newSikaTentyo = true;
			}

			if (doCameraMovieTimer > 3.5f && doCameraMovieTimer < 5.8f && cameraAngle.x > DirectX::XMConvertToRadians(-40))
				cameraAngle.x -= DirectX::XMConvertToRadians(20) * elapsedTime;
			else if(doCameraMovieTimer >= 5.8f && cameraAngle.x < DirectX::XMConvertToRadians(0))
				cameraAngle.x += DirectX::XMConvertToRadians(50) * elapsedTime;

			if (doCameraMovieTimer > 6.5f && !setCameraShake[0])
			{
				std::unique_ptr<Enemy>& sikaTentyo = EnemyManager::Instance().GetEnemy(0);

				shockWaveEffect->Play(sikaTentyo->GetCollisionPos(), 0.8f);

				SoundEffectManager::Instance().PlaySoundEffect("着地");

				cameraController->SetCameraShake(0.4f, DirectX::XMINT3(0, 30, 0));
				setCameraShake[0] = true;
			}

			if (doCameraMovieTimer > 9.0f && doCameraMovieTimer < 9.8f && cameraRange > 5.0f)
				cameraRange -= 20 * elapsedTime;
			else if (doCameraMovieTimer >= 9.8f && cameraRange < 13.0f)
			{
				cameraRange += 25 * elapsedTime;

				if (!setCameraShake[1])
				{
					SoundEffectManager::Instance().PlaySoundEffect("鳴き声");
					cameraController->SetCameraShake(2.0f, DirectX::XMINT3(0, 30, 0));
					setCameraShake[1] = true;
				}
			}

			if (doCameraMovieTimer >= 12.5f)
				cameraAngle = { DirectX::XMFLOAT3(DirectX::XMConvertToRadians(18),
					0,0)
			};

			cameraController->SetAngle(cameraAngle);
			target = cameraTarget;
			cameraController->SetRange(cameraRange);
		}
	}
}
