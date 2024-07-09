#include "SceneClear.h"
#include "Camera.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneGameOver.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include <Graphics\Graphics.h>

// 初期化
void SceneClear::Initialize()
{
	// スコア
	text[0] = std::make_unique<Text>();
}

// 終了化
void SceneClear::Finalize()
{
}

// 更新処理
void SceneClear::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_START;
	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneLoading* loadingScene = new SceneLoading(new SceneTitle);

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(loadingScene);
	}
}

// 描画処理
void SceneClear::Render()
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

	SceneTitle& title = SceneTitle::Instance();

	text[0]->Render(dc,
		true, false,
		true,
		0, 0, 0, 0, 0,
		0, 0, 0, title.score,
		800, 360,
		12, 12,
		0,
		40,
		1, 1, 1, 1);
}