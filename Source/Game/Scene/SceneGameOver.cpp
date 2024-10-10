#include "SceneGameOver.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneTitle.h"
#include "Input/Input.h"

// 初期化
void SceneGameOver::Initialize()
{
	saru = std::make_unique<Sprite>("Data/Sprite/背景/Saru3.png");
	ganban = std::make_unique<Sprite>("Data/Sprite/背景/岩盤.png");
	ganbanHekomi = std::make_unique<Sprite>("Data/Sprite/背景/岩盤凹み.png");
}

// 終了化
void SceneGameOver::Finalize()
{
}

// 更新処理
void SceneGameOver::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}

	if(hekomiScale.x < 1280.0f)
		hekomiScale.x += 1600 * elapsedTime;

	if (hekomiScale.y < 720.0f)
		hekomiScale.y += 900 * elapsedTime;

	if (hekomiPos.x > 0)
		hekomiPos.x -= 775 * elapsedTime;

	if (hekomiPos.y > 0)
		hekomiPos.y -= 370 * elapsedTime;
}

// 描画処理
void SceneGameOver::Render()
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

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	float textureWidth = static_cast<float>(ganban->GetTextureWidth());
	float textureHeight = static_cast<float>(ganban->GetTextureHeight());

	ganban->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	ganbanHekomi->Render(dc,
		hekomiPos.x, hekomiPos.y,
		hekomiScale.x, hekomiScale.y,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	saru->Render(dc,
		saruPos.x, saruPos.y,
		saruScale.x, saruScale.y,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, 1);
}
