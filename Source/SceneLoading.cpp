#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include <EnemyStrong.h>
#include <EnemyManager.h>
#include <Camera.h>
#include <SceneTitle.h>
#include <EnemyStrongLoading.h>

// 初期化
void SceneLoading::Initialize()
{
	// スプライト初期化
	sprite = std::make_unique<Sprite>("Data/Sprite/LoadingIcon.png");

	// スレッド開始
	thread = std::make_unique<std::thread>(LoadingThread, this);

	for (int i = 0; i < 4; i++)
	{
		std::string filePath = "Data/Sprite/Tips" + std::to_string(i + 1) + ".png";
		mozi[i] = std::make_unique<Sprite>(filePath.c_str());
	}

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

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	EnemyStrongLoading* strong = new EnemyStrongLoading();
	strong->SetPosition(DirectX::XMFLOAT3(8.5f, 5.5f, 0));
	enemyManager.Register(strong);
}

// 終了化
void SceneLoading::Finalize()
{
	//if (sprite != nullptr)
	//{
	//	delete sprite;
	//	sprite = nullptr;
	//}

	//// スレッド終了化
	//if (thread != nullptr)
	//{

	//	/*delete thread;
	//	thread = nullptr;*/
	//}
	if (thread->joinable())
	{
		thread->join(); // スレッドの終了を待つ
	}

	// エネミー終了化
	EnemyManager::Instance().Clear();
}

// 更新処理
void SceneLoading::Update(float elapsedTime)
{
	SceneTitle& title = SceneTitle::Instance();
	title.gameLoading = true;

	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target;
	target = { 0 ,3.1f, 0 };
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	constexpr float speed = 180;
	angle += speed * elapsedTime;

	// 次のシーンの準備が完了したらシーンを切り替える
	if (nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (((gamePad.GetButtonDown() & GamePad::BTN_START) ||
		(gamePad.GetButtonDown() & GamePad::BTN_A))
		&& nextMozi == true)
	{
		moziNum += 1;
	}
		

	if (moziNum > 4) moziNum = 1;

	switch (moziNum)
	{
	case 1:
		moziView[3] -= 0.025f;
		moziView[0] += 0.005f;
		if (moziView[0] >= 1) nextMozi = true;
		else nextMozi = false;

		break;
	case 2:
		moziView[0] -= 0.025f;
		moziView[1] += 0.005f;
		if (moziView[1] >= 1) nextMozi = true;
		else nextMozi = false;

		break;
	case 3:
		moziView[1] -= 0.025f;
		moziView[2] += 0.005f;
		if (moziView[2] >= 1) nextMozi = true;
		else nextMozi = false;

		break;
	case 4:
		moziView[2] -= 0.025f;
		moziView[3] += 0.005f;
		if (moziView[3] >= 1) nextMozi = true;
		else nextMozi = false;
	default:
		break;
	}

	for (int i = 0; i < 4; i++)
	{
		if (moziView[i] < 0)
			moziView[i] = 0;

		if (moziView[i] > 1)
			moziView[i] = 1;
	}

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア&レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //RGBA(0.0～1.0)
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

	for (int i = 0; i < 4; i++)
	{
		mozi[i]->Render(dc,
			0, 0,
			1280, 720,
			0, 0,
			1280, 720,
			0,
			1, 1, 1, moziView[i]);
	}

	//! 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		// エネミー描画
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);

	}
}

//ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM関数の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	// 次のシーンの初期化を行う
	if (scene->nextScene != nullptr)
	{
		scene->nextScene->Initialize();
	}

	// スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	// 次のシーンの準備完了設定
	scene->nextScene->SetReady();
	//scene->SetReady();
}