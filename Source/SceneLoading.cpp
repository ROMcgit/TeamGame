#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include <EnemyStrong.h>
#include <EnemyManager.h>
#include <Camera.h>

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

	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	EnemyStrong* strong = new EnemyStrong();
	strong->SetPosition(DirectX::XMFLOAT3(pos.x, pos.y, 0));
	strong->SetTerritory(strong->GetPosition(), 10.0f);
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

	//{
	//	if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
	//	{
	//		ImGui::DragFloat2("Pos", &pos.x);
	//	}
	//	ImGui::End;
	//}
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