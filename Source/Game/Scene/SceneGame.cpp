#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemySika.h"
#include "Game/Character/Item/ItemManager.h"
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
		DirectX::XMConvertToRadians(20),
		0, 0)
	);
}

// 終了化
void SceneGame::Finalize()
{
	// ステージ終了化
	StageManager::Instance().Clear();

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
		EnemyManager::Instance().SpriteRender(dc);
		EnemyManager::Instance().RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2DデバッグGUI描画
	{
		// プレイヤーデバッグ描画
		player->DrawDebugGUI();
		camera.DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}

// 敵とアイテム生成処理
void SceneGame::NewEnemyOrItem()
{
	EnemyManager& enemyManager = EnemyManager::Instance();
	ItemManager&  itemManager  = ItemManager::Instance();

	int enemyCount         = enemyManager.GetEnemyCount();
	int itemCount          = itemManager.GetItemCount();
	int establishmentCount = enemyCount + itemCount;

	if (establishmentCount < 20)
	{
		int ransu = rand() % 7 + 1;

		switch (ransu)
		{
		//! 敵生成
		case 1:
		case 2:
		case 3:
		case 4:
		{
			std::unique_ptr<EnemySika> sika = std::make_unique<EnemySika>();
			sika->SetPosition(DirectX::XMFLOAT3(20, 0, 20));
			enemyManager.Register(std::move(sika));
		}
			break;
		//! アイテム生成
		case 5:
		case 6:
			break;
		//! 設置物生成
		case 7:
		{

		}
		default:
			break;
		}
		
	}

}
