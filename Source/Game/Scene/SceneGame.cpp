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
#include "Game/Character/Item/Apple.h"
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

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// アイテム更新処理
	ItemManager::Instance().Update(elapsedTime);

	// 設置物更新処理
	InstallationManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// フェードのアップデート
	fade->Update(elapsedTime);

	if (player->GetHealth() <= 0)
	{
		if (!setFade)
		{
			fade->SetFade(DirectX::XMFLOAT3(1, 1, 1),
				0.0f, 1.0f,
				2.5f);

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
		
		// アイテム描画処理
		ItemManager::Instance().Render(dc, shader);

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
	}

	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::InputInt("EstablishmentCount", &establishmentCount); // 設置した数
			ImGui::InputFloat("NewestablishmentTimer", &newestablishmentTimer); // 設置した数

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
	establishmentCount = enemyCount + itemCount + installationCount;     // 全ての数

	if (newestablishmentTimer > newestablishmentMaxTimer)
	{
		float angleY = player->GetAngle().y;

		// 生成する範囲の角度をランダムに決める
		float randomAngle = angleY + (rand() % 360 - 180) * (3.14159f / 180.0f); // -180度から+180度までランダム

		float distance = rand() % 20 + 20; // プレイヤーからの距離もランダム

		int posX = player->GetPosition().x + distance * cos(randomAngle); // cosでX座標を計算
		int posZ = player->GetPosition().z + distance * sin(randomAngle); // sinでZ座標を計算

		if (establishmentCount < 35)
		{
			int newRansu = rand() % 10 + 1;

			switch (newRansu)
			{
				//! 敵生成
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			{
				std::unique_ptr<EnemySika> sika = std::make_unique<EnemySika>();
				sika->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				enemyManager.Register(std::move(sika));

				newestablishmentTimer = 0.0f;
			}
			break;
			case 7:
			case 8:
			case 9:
				//! アイテム生成
			{
				std::unique_ptr<Apple> apple = std::make_unique<Apple>();
				apple->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(apple));

				newestablishmentTimer = 0.0f;
			}
			break;
			case 10:
				//! 設置物生成
			{


				newestablishmentTimer = 0.0f;
			}
			break;
			default:
				break;
			}
		}
		else
			newestablishmentTimer = 0;
	}
	else
		newestablishmentTimer += elapsedTime;
	
}
