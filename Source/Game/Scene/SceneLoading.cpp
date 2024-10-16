#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Game/Scene/SceneManager.h"

// 初期化
void SceneLoading::Initialize()
{
	// バナナ
	banana = std::make_unique<Sprite>("Data/Sprite/Banana.png");

	// ローディング
	for (int i = 0; i < 4; i++)
	{
		std::string filePath = "Data/Sprite/Loading" + std::to_string(i + 1) + ".png";

		loading[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	// スレッド開始
	thread = std::make_unique <std::thread>(LoadingThread, this);
}

// 終了化
void SceneLoading::Finalize()
{
	if (thread->joinable())
	{
		thread->join(); // スレッドの終了を待つ
	}
}

// 更新処理
void SceneLoading::Update(float elapsedTime)
{
	//! バナナ回転速度
	angle -= rotationSpeed * elapsedTime;

	//! バナナの位置
	bananaPos.x -= moveSpeed * elapsedTime;

	if (bananaPos.x < -250)
	{
		rotationSpeed += 300;
		moveSpeed += 300;
		bananaPos.x = 1350;
	}
	
	if (rotationSpeed > 6000)
		rotationSpeed = 6000;

	if (moveSpeed > 6000)
		moveSpeed = 6000;

//--------------------------------------------------------------------------------------------//

	spriteChangeTimer += elapsedTime;

	if (spriteChangeTimer > 0.3)
	{
		spriteChangeTimer = 0.0f;
		spriteNum++;
	}

	if (spriteNum > 3)
		spriteNum = 0;

	// 次のシーンの準備が完了したらシーンを切り替える
	if (nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(std::move(nextScene));
	}
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

	// 2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(banana->GetTextureWidth());
		float textureHeight = static_cast<float>(banana->GetTextureHeight());
		// バナナ描画
		banana->Render(dc,
			bananaPos.x, bananaPos.y, 260, 220,
			0, 0, textureWidth, textureHeight,
			angle,
			1, 1, 1, 1);

		textureWidth  = static_cast<float>(loading[0]->GetTextureWidth());
		textureHeight = static_cast<float>(loading[0]->GetTextureHeight());

		// ローディング
		loading[spriteNum]->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
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
	if (scene->nextScene != nullptr)
	{
		scene->nextScene->SetReady();
	}
	//scene->SetReady();
}