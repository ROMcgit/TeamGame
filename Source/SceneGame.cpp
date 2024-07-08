#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySphere.h"
#include "WallManager.h"
#include "WallEnemy.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"
#include "SceneLoading.h"
#include "SceneGameOver.h"
#include "SceneManager.h"
#include "SceneClear.h"

Sprite* EnemyHp;

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	//stage = new Stage();
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
	//StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	//stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	//stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	//stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	//stageManager.Register(stageMoveFloor);

	EnemyHp = new Sprite;

	// プレイヤー初期化
	player = std::make_unique<Player>();

	// HP
	uiSprite[1] = std::make_unique<Sprite>();
	uiSprite[2] = std::make_unique<Sprite>();

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
	cameraController = new CameraController;
	cameraController->SetAngle(DirectX::XMFLOAT3(
		DirectX::XMConvertToRadians(12),
		0,
		0));

	// エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	//Enemy* enemy = new EnemySlime;
	// slime->SetPosition(DirectX::XMFLOAT3(0,0,5))
	//enemyManager.Register(enemy);

	for (int i = 0; i < 2; ++i)
	{
		EnemySphere* sphere = new EnemySphere();
		sphere->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 0));
		sphere->SetTerritory(sphere->GetPosition(), 10.0f);
		enemyManager.Register(sphere);
	}

	WallManager& wallManager = WallManager::Instance();

	for (int i = 0; i < 2; ++i)
	{
		WallEnemy* wall = new WallEnemy();
		wall->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 0));
		wallManager.Register(wall);
	}

	// ゲージスプライト
	gauge = new Sprite();
	
}

// 終了化
void SceneGame::Finalize()
{
	// ゲージスプライト終了化
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// //ステージ終了化
	//if (stage != nullptr)
	//{
	//	delete stage;
	//	stage = nullptr;
	//}
	StageManager::Instance().Clear();

	//// プレイヤー終了化
	//if (player != nullptr)
	//{
	//	delete player;
	//	player = nullptr;
	//}

	// エネミー終了化
	EnemyManager::Instance().Clear();

	// 壁終了化
	WallManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target;
	target = { 0 ,3.1f, 0 };
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	//stage->Update(elapsedTime);
	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// 壁の更新処理
	WallManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	if ( enemyCount <= 0)
	{
		SceneLoading* loadingScene = new SceneLoading(new SceneClear);

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(loadingScene);
	}

	if (player->GetHealth() <= 0)
	{
		SceneLoading* loadingScene = new SceneLoading(new SceneGameOver);

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(loadingScene);
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

	//! 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// ステージ描画
		//stage->Render(dc, shader);
		StageManager::Instance().Render(dc, shader);

		// プレイヤー描画
		player->Render(dc, shader);

		// エネミー描画
		EnemyManager::Instance().Render(dc,shader);

		// 壁描画
		WallManager::Instance().Render(dc, shader);

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

		// 壁デバッグプリミティブ描画
		WallManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		PlayerUI();

		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2DデバッグGUI描画
	{
		// プレイヤーデバッグ描画
		player->DrawDebugGUI();
		camera.DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}

// プレイヤーUI
void SceneGame::PlayerUI()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	float textureWidth = static_cast<float>(uiSprite[2]->GetTextureWidth());
	float textureHeight = static_cast<float>(uiSprite[2]->GetTextureHeight());

	// ダメージゲージ
	uiSprite[2]->Render(dc,
		20, 20, 
		player->GetDamageHealth() * 0.9f, 25,
		0, 0, textureWidth, textureHeight,
		0,
		1, 0, 0, 1);

	textureWidth = static_cast<float>(uiSprite[1]->GetTextureWidth());
	textureHeight = static_cast<float>(uiSprite[1]->GetTextureHeight());

	// HPゲージ
	uiSprite[1]->Render(dc,
		20, 20,
		player->GetHealth() * 0.9f, 25,
		0, 0, textureWidth, textureHeight,
		0,
		0, 1, 0, 1);
}

// エネミーHPゲージ描画
void SceneGame::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// 全ての敵の頭上にHPゲージを表示
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 敵の位置を取得
		DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();

		// 位置を変換
		DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&enemyPos);
		DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project(
			Pos,
			viewport.TopLeftX, 
			viewport.TopLeftY,
			viewport.Width, 
			viewport.Height,
			viewport.MinDepth, 
			viewport.MaxDepth,
			Projection,
			View, 
			World);

		// スクリーン座標を取得
		DirectX::XMFLOAT3 screenPos;
		DirectX::XMStoreFloat3(&screenPos, ScreenPos);

		// HPゲージの描画位置
		float gaugeWidth = enemy->GetHealth() * 8;  // HPゲージの幅
		float gaugeHeight = 5.0f;  // HPゲージの高さ
		float gaugeX = screenPos.x - gaugeWidth / 2;
		float gaugeY = screenPos.y - gaugeHeight - 55.0f; // 少し上にオフセット

		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Unproject(
			ScreenPos,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 worldPos;
		DirectX::XMStoreFloat3(&worldPos, WorldPosition);

		EnemyHp->Render(dc,
			gaugeX, //dx
			gaugeY, //dy
			gaugeWidth, //dw
			gaugeHeight, //dh
			0,           //sx
			0,           //sy
			gaugeWidth,  //sw
			gaugeHeight, //sh
			0,			 //angle
			1, 0, 0, 1   //color
		);
	}
}