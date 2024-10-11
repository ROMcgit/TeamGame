#include "SceneGameClear.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneTitle.h"
#include "Input/Input.h"

// 初期化
void SceneGameClear::Initialize()
{
	//! 空
	backGround = std::make_unique<Sprite>("Data/Sprite/背景/空.png");

	//! ショッピングモール
	shopping = std::make_unique <Sprite>("Data/Sprite/背景/ショッピングモール.png");

	//! ショッピングモール(崩壊)
	collapseShopping = std::make_unique<Sprite>("Data/Sprite/背景/ショッピングモール(崩壊).png");

	//! シカ
	sika = std::make_unique<Sprite>("Data/Sprite/背景/シカ2.png");
}

// 終了化
void SceneGameClear::Finalize()
{
}

// 更新処理
void SceneGameClear::Update(float elapsedTime)
{
	sikaPosY  += 300 * elapsedTime;
	sikaAngle += DirectX::XMConvertToRadians(5000) * elapsedTime;

	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneTitle>());

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// 描画処理
void SceneGameClear::Render()
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

	float textureWidth = static_cast<float>(backGround->GetTextureWidth());
	float textureHeight = static_cast<float>(backGround->GetTextureHeight());

	//! 背景
	backGround->Render(dc,
		0, 0,
		screenWidth, screenHeight,
		0, 0,
		textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	if (spriteScene == SpriteScene::SikaDown)
	{
		textureWidth = static_cast<float>(shopping->GetTextureWidth());
		textureHeight = static_cast<float>(shopping->GetTextureHeight());

		//! ショッピングモール
		shopping->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		textureWidth = static_cast<float>(sika->GetTextureWidth());
		textureHeight = static_cast<float>(sika->GetTextureHeight());

		//! シカ
		sika->Render(dc,
			650, sikaPosY,
			10, 10,
			0, 0,
			textureWidth, textureHeight,
			sikaAngle,
			1, 1, 1, 1);
	}

	if (spriteScene == SpriteScene::CollapseShoppingMall)
	{

	}
}
