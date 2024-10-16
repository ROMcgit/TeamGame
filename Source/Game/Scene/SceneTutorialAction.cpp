#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneTutorialAction.h"
#include "SceneGameOver.h"
#include "SceneGameClear.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneOpning.h"
#include "SceneManager.h"

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
#include "Game/Character/Item/Banana.h"

#include "Game/Character/Installation/InstallationManager.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/StageMain.h"
#include "SceneTitle.h"

// 初期化
void SceneTutorialAction::Initialize()
{
	for (int i = 0; i < 11; i++)
	{
		std::string filePath = "Data/Sprite/チュートリアル/チュートリアル" + std::to_string(i + 1) + ".png";
		image[i] = std::make_unique<Sprite>(filePath.c_str());
	}

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
	backGround = std::make_unique<Sprite>();

	fade = std::make_unique<Fade>();
}

// 終了化
void SceneTutorialAction::Finalize()
{
}

// 更新処理
void SceneTutorialAction::Update(float elapsedTime)
{
}

// 描画処理
void SceneTutorialAction::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア&レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; //RGBA(0.0～1.0)
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

	// 2Dスプライト描画
	{
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
			0, 0, 0, 1);
	}

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// ステージ描画
		StageManager::Instance().Render(dc, shader);

		SceneTitle& scene = SceneTitle::Instance();

		// プレイヤー描画
		player->Render(dc, shader);

		// アイテム描画処理
		ItemManager::Instance().Render(dc, shader);

		// 重要アイテム描画処理
		ImportantItemManager::Instance().Render(dc, shader);

		// 設置物描画処理
		InstallationManager::Instance().Render(dc, shader);

		shader->End(dc);
	}
}