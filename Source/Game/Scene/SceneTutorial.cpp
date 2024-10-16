#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneTutorial.h"
#include "SceneTutorialAction.h"
#include "SceneGameOver.h"
#include "SceneGameClear.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneOpning.h"
#include "SceneManager.h"

// 初期化
void SceneTutorial::Initialize()
{
	for (int i = 0; i < 11; i++)
	{
		std::string filePath = "Data/Sprite/チュートリアル/チュートリアル" + std::to_string(i + 1) + ".png";
		image[i] = std::make_unique<Sprite>(filePath.c_str());
	}
}

// 終了化
void SceneTutorial::Finalize()
{
}

// 更新処理
void SceneTutorial::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTutorialAction>());
		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// 描画処理
void SceneTutorial::Render()
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

	// 2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(image[0]->GetTextureWidth());
		float textureHeight = static_cast<float>(image[0]->GetTextureHeight());
		// タイトルスプライト描画
		image[imageNum - 1]->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
}