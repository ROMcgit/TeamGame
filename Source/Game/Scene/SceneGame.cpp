#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemySlime.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/StageMain.h"
#include "Game/Stage/StageMoveFloor.h"

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	//stage = new Stage();
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
	StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);

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

	//カメラコントローラー初期化
	cameraController = std::make_unique <CameraController>();

	// エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	//Enemy* enemy = new EnemySlime;
	// slime->SetPosition(DirectX::XMFLOAT3(0,0,5))
	//enemyManager.Register(enemy);

	for (int i = 0; i < 2; ++i)
	{
		std::unique_ptr<EnemySlime> slime = std::make_unique <EnemySlime>();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		slime->SetTerritory(slime->GetPosition(), 10.0f);
		enemyManager.Register(std::move(slime));
	}
	
	enemyHp = std::make_unique <Sprite>();

}

// 終了化
void SceneGame::Finalize()
{
	//if (enemyHp != nullptr)
	//{
	//	delete enemyHp;
	//	enemyHp = nullptr;
	//}

	// カメラコントローラー終了化
	//if (cameraController != nullptr)
	//{
	//	delete cameraController;
	//	cameraController = nullptr;
	//}

	// //ステージ終了化
	//if (stage != nullptr)
	//{
	//	delete stage;
	//	stage = nullptr;
	//}

	StageManager::Instance().Clear();

	// プレイヤー終了化
	//if (player != nullptr)
	//{
	//	delete player;
	//	player = nullptr;
	//}

	// エネミー終了化
	EnemyManager::Instance().Clear();

}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	//stage->Update(elapsedTime);
	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
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

	//// ビュー行列
	//{
	//	DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// カメラの視点（位置）
	//	DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// カメラの注視点（ターゲット）
	//	DirectX::XMFLOAT3 up = { 0, 1, 0 };		// カメラの上方向

	//	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	//	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	//	DirectX::XMStoreFloat4x4(&rc.view, View);
	//}
	//// プロジェクション行列
	//{
	//	float fovY = DirectX::XMConvertToRadians(45);	// 視野角
	//	float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// 画面縦横比率
	//	float nearZ = 0.1f;	// カメラが映し出すの最近距離
	//	float farZ = 1000.0f;	// カメラが映し出すの最遠距離
	//	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	//	DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	//}

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

	// 2Dスプライト描画
	{
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
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

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

		enemyHp->Render(dc,
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

#if 0
		// エネミー配置処理
		Mouse& mouse = Input::Instance().GetMouse();
		if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
		{
			// マウスカーソル座標を取得
			DirectX::XMFLOAT3 screenPosition;
			screenPosition.x = static_cast<float>(mouse.GetPositionX());
			screenPosition.y = static_cast<float>(mouse.GetPositionY());
			screenPosition.z = 1.0f;

			// スクリーン座標をワールド座標に変換
			DirectX::XMVECTOR ScreenPos = DirectX::XMLoadFloat3(&screenPosition);
			DirectX::XMVECTOR WorldPos = DirectX::XMVector3Unproject(
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

			// 地面との交差点を計算
			DirectX::XMFLOAT3 worldPos;
			DirectX::XMStoreFloat3(&worldPos, WorldPos);

			Camera& camera = Camera::Instance();

			// カメラの位置と注視点からレイを計算
			DirectX::XMFLOAT3 cameraPos = camera.GetEye();
			DirectX::XMFLOAT3 cameraFocus = camera.GetFocus();
			DirectX::XMVECTOR rayOrigin = DirectX::XMLoadFloat3(&cameraPos);
			DirectX::XMVECTOR rayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(WorldPos, DirectX::XMLoadFloat3(&cameraFocus)));

			// 地面はy=0平面と仮定し、交差点を計算
			float t = -DirectX::XMVectorGetY(rayOrigin) / DirectX::XMVectorGetY(rayDir);
			DirectX::XMVECTOR groundIntersect = DirectX::XMVectorMultiply(DirectX::XMVectorAdd(rayOrigin, DirectX::XMVectorReplicate(t)), rayDir);

			// 新しいエネミーの位置を設定
			DirectX::XMStoreFloat3(&worldPos, groundIntersect);

			// カメラのインスタンス取得
			Camera& camera = camera.Instance();

			DirectX::XMFLOAT3 start = camera.GetEye(); // レイの開始点はカメラの位置
			DirectX::XMFLOAT3 end = worldPos; // レイの終了点はマウスカーソルのワールド座標

			HitResult hitResult;
			if (StageManager::Instance().RayCast(start, end, hitResult))
			{
				worldPos = hitResult.position; // レイがヒットした位置を新しいエネミーの位置として使用
			}

			// エネミーを配置
			std::unique_ptr<Enemy> newEnemy = std::make_unique<EnemySlime>(); // 新しいエネミーを生成
			newEnemy->SetPosition(worldPos); // 位置を設定
			enemyManager.Register(std::move(newEnemy)); // エネミーをマネージャーに追加
		}
	}
#endif
	}
}