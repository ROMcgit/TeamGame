#include "Graphics/Graphics.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/SceneManager.h"
#include "Other/Easing.h"

#include "Game/Scene/G0_Onigokko_Tutorial.h"
#include "Game/Scene/G1_DarumasangaKoronda_Tutorial.h"
#include "Game/Scene/G2_Sundome_Tutorial.h"
#include "Game/Scene/G3_SoratobuHusenWari_Tutorial.h"
#include "Game/Scene/G4_OssanTataki_Tutorial.h"
#include "Game/Scene/G5_Asibawatari_Tutorial.h"

#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Stage/StageGameSelect.h"
#include "Game/Stage/StageMoveFloor.h"

#include "Game/Character/GameSelect/GameSelectManager.h"
#include "Game/Character/GameSelect/GS0_Onigokko.h"
#include "Game/Character/GameSelect/GS1_DarumasangaKoronda.h"
#include "Game/Character/GameSelect/GS2_Sundome.h"
#include "Game/Character/GameSelect/GS3_SoratobuHusenWari.h"
#include "Game/Character/GameSelect/GS4_OssanTataki.h"
#include "Game/Character/GameSelect/GS5_AsibaWatari.h"

#include <algorithm>

SceneGameSelect::GameSelectA SceneGameSelect::gameSelect = SceneGameSelect::GameSelectA::Onigokko;
bool SceneGameSelect::sceneChange = false;

// クリア
SceneGameSelect::Clear SceneGameSelect::clear;

// 初期化
void SceneGameSelect::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();
	
	// レンダーターゲット
	renderTarget = std::make_unique<RenderTarget>(device, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	// シャドウマップ初期化
	shadowMap.Initialize();

/********************************************************************************/

	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	std::unique_ptr<StageGameSelect> stageMain = std::make_unique<StageGameSelect>();
	stageManager.Register(std::move(stageMain));

	// プレイヤー初期化
	player = std::make_unique<Player0_Onigokko>();
	player->SetPosition(DirectX::XMFLOAT3(0, -5.2f, 0));
	player->SetScale(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));
	player->SetMoveSpeed(20.0f);
	player->SetRadius(1.9f);
	player->SetHeight(16.0f);

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

	//! ポストエフェクトの設定(一回だけ)フラグを初期化
	camera.SetPostEffectStatusOnceUnlock();

	//カメラコントローラー初期化
	cameraController = std::make_unique <CameraController>();
	cameraController->SetRange(85.5f);
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(40), 0, 0));

	// 背景
	backGround = std::make_unique<Sprite>();

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);

	sceneChange = false;

	//! ゲームの選択画面
	for(int i = 0; i < 6; i++)
	{
		std::string filePath = "Data/Sprite/GameSelect/" + std::to_string(i) + ".png";
		gameSelectSprite[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	float setPosX = screenWidth * 0.5f;
	//! ボーナス画像
	for(int i = 0; i < 7; i++)
	{
		std::string filePath = "";

		switch (i + 1)
		{
		case 1: filePath = "Data/Sprite/0.Onigokko/Bonus.png";           break;
		case 2: filePath = "Data/Sprite/1.DarumasangaKoronda/Bonus.png"; break;
		case 3: filePath = "Data/Sprite/2.Sundome/Bonus.png";            break;
		case 4: filePath = "Data/Sprite/3.SoratobuHusenWari/Bonus.png";  break;
		case 5: filePath = "Data/Sprite/4.OssanTataki/Bonus.png";        break;
		case 6: filePath = "Data/Sprite/5.Asibawatari/Bonus.png";        break;
		case 7: filePath = "Data/Sprite/GameComplete.png";               break;
		default:
			break;
		}

		bonusImage[i] = std::make_unique<Sprite>(filePath.c_str());

		bonusImagePosX[i] = setPosX;
		setPosX += screenWidth;

		bonusImageFrame[i] = std::make_unique<Sprite>();

		filePath = "Data/Sprite/Bonus/Hint" + std::to_string(i) + ".png";
		hint[i] = std::make_unique<Sprite>(filePath.c_str());
	}
	

	GameSelectManager& gameSelectManager = GameSelectManager::Instance();

	float posX = 65.0f;

	//! おにごっこ
	std::unique_ptr<GS0_OniGokko> onigokko = std::make_unique<GS0_OniGokko>();
	onigokko->SetPosition(DirectX::XMFLOAT3(posX * -2.5f, 1, 80));
	gameSelectManager.Register(std::move(onigokko));

	//! だるまさんが転んだ
	std::unique_ptr<GS1_DarumasangaKoronda> darumasangaKoronda = std::make_unique<GS1_DarumasangaKoronda>();
	darumasangaKoronda->SetPosition(DirectX::XMFLOAT3(posX * -1.5f, 1, 80));
	gameSelectManager.Register(std::move(darumasangaKoronda));

	//! 寸止め
	std::unique_ptr<GS2_Sundome> sundome = std::make_unique<GS2_Sundome>();
	sundome->SetPosition(DirectX::XMFLOAT3(posX * -0.5f, 1, 80));
	gameSelectManager.Register(std::move(sundome));

	//! 空飛ぶ風船割り
	std::unique_ptr<GS3_SoratobuHusenWari> soratobuHusenWari = std::make_unique<GS3_SoratobuHusenWari>();
	soratobuHusenWari->SetPosition(DirectX::XMFLOAT3(posX * 0.5f, 1, 80));
	gameSelectManager.Register(std::move(soratobuHusenWari));

	//! おっさん叩き
	std::unique_ptr<GS4_OssanTataki> ossanTataki = std::make_unique<GS4_OssanTataki>();
	ossanTataki->SetPosition(DirectX::XMFLOAT3(posX * 1.5f, 1, 80));
	gameSelectManager.Register(std::move(ossanTataki));

	//! 足場渡り
	std::unique_ptr<GS5_AsibaWatari> asibaWatari = std::make_unique<GS5_AsibaWatari>();
	asibaWatari->SetPosition(DirectX::XMFLOAT3(posX * 2.5f, 1, 80));
	gameSelectManager.Register(std::move(asibaWatari));
}

// 終了化
void SceneGameSelect::Finalize()
{
	//! ステージを破棄する
	StageManager::Instance().Clear();

	//! 敵を全て破棄する
	EnemyManager::Instance().Clear();

	//! エフェクトを全て破棄する
	EffectManager::Instance().Cleanup();

	GameSelectManager::Instance().Clear();
}

// 更新処理
void SceneGameSelect::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y = player->GetHeight() * 0.5f;

	if (target.x < -126.0f || target.x > 126.0f)
	{
		target.x = std::clamp(target.x, -126.0f, 126.0f);
	}

	if (target.z < -35.0f || target.z > 35.0f)
	{
		target.z = std::clamp(target.z, -35.0f, 35.0f);
	}

	cameraController->SetTarget(target);
	Camera::Instance().Update(elapsedTime);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	// プレイヤーの位置制限
	PlayerPositionControll();
	// プレイヤー更新処理
	if(!viewBonusImage)
		player->Update(elapsedTime);

	fade->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	GameSelectManager::Instance().Update(elapsedTime);

	// ボーナス画像の更新処理
	UpdateBonusImage(elapsedTime);

	if (sceneChange)
	{
		player->SetMovieTime(1.0f);

		if (!setFade)
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				1.0f, 0.2f);

			setFade = true;
		}

		if (setFade && !fade->GetFade())
		{
			std::unique_ptr<SceneLoading> loadingScene;

			switch (gameSelect)
			{
			case GameSelectA::Onigokko:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G0_Onigokko_Tutorial>());
				break;
			case GameSelectA::DarumasangaKoronda:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G1_DarumasangaKoronda_Tutorial>());
				break;
			case GameSelectA::Sundome:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G2_Sundome_Tutorial>());
				break;
			case GameSelectA::SoratobuHusenWari:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G3_SoratobuHusenWari_Tutorial>());
				break;
			case GameSelectA::OssanTataki:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G4_OssanTataki_Tutorial>());
				break;
			case GameSelectA::Asibawatari:
				loadingScene = std::make_unique<SceneLoading>(std::make_unique<G5_Asibawatari_Tutorial>());
				break;
			default:
				break;
			}

			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
}

// 描画処理
void SceneGameSelect::Render()
{
	lightPosition.x = CameraController::target.x;
	lightPosition.y = 5.0f;
	lightPosition.z = CameraController::target.z - 25.0f;
	lightRange = 20000.0f;

	shadowMapEyeOffset = { 3.0f, 22.0f, 9.0f };

	//! フォグ
	fogStart = 100.0f;
	fogEnd   = 150.0f;

	Graphics& graphics = Graphics::Instance();
	
	DrawingSettings(graphics);

	//! シャドウマップ
	{
		// ボーナス画像を表示しないなら
		if ((!viewBonusImage) || (viewBonusImage && fade->GetFadeOpacity() < 1.0f))
		{
			//! シャドウマップ開始
			shadowMap.Begin(rc);
			{
				Shader* shadowMapShader = graphics.GetShadowMapShader();
				shadowMapShader->Begin(dc, rc);

				//エネミー描画
				EnemyManager::Instance().Render(dc, shadowMapShader);
				// プレイヤー描画
				player->Render(dc, shadowMapShader);

				shadowMapShader->End(dc);
			}
			//! シャドウマップ終了
			shadowMap.End();
		}
	}

	//! レンダーターゲット
	{
		if ((!viewBonusImage) || (viewBonusImage && fade->GetFadeOpacity() < 1.0f))
		//! レンダーターゲットに描画開始
		renderTarget->Begin();
	}

	//! 2Dスプライト
	{
		// 深度を無効にする
		ID3D11DepthStencilState* depthDisabledState = graphics.GetDepthDisabledState();
		dc->OMSetDepthStencilState(depthDisabledState, 0);

		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(backGround->GetTextureWidth());
		float textureHeight = static_cast<float>(backGround->GetTextureHeight());

		//! 背景描画
		backGround->Render(dc,
			0, 0, screenWidth, screenHeight * 0.8f,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		ID3D11DepthStencilState* depthEnabledState = graphics.GetDepthEnabledState();
		dc->OMSetDepthStencilState(depthEnabledState, 0);
	}

	// 3Dモデル描画
	{
		// ボーナス画像を表示しないなら
		if ((!viewBonusImage) || (viewBonusImage && fade->GetFadeOpacity() < 1.0f))
		{
			Shader* shader = graphics.GetDefaultLitShader();
			shader->Begin(dc, rc);
			// ステージ描画
			StageManager::Instance().Render(dc, shader);

			// カメラの位置を描画
			CameraController::Instance().RenderCameraTarget(dc, shader);

			GameSelectManager::Instance().Render(dc, shader);

			// プレイヤー描画
			player->Render(dc, shader);

			//エネミー描画
			EnemyManager::Instance().Render(dc, shader);
			shader->End(dc);
		}
	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

#ifdef _DEBUG

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		player->DrawDebugPrimitive();

		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().DrawDebugPrimitive();

		GameSelectManager::Instance().DrawDebugPrimitive();
	}
#endif // DEBUG

	// ラインレンダラ描画実行
	graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

	// デバッグレンダラ描画実行
	graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

	//! シェーダーを出す
	{
		if ((!viewBonusImage) || (viewBonusImage && fade->GetFadeOpacity() < 1.0f))
		{
			//! レンダーターゲットへ描画終了
			renderTarget->End();
			//! スクリーンをポストエフェクトシェーダーで描画
			Camera::Instance().CreatePostEffect();
			Camera::Instance().SetPostEffectStatusOnce();
			//! スクリーンをポストエフェクトシェーダーで描画
			renderTarget->Render();
		}
	}

	{
		RenderGameSprite(dc, rc.view, rc.projection);

		fade->Render(dc, graphics);

		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		
		for (int i = 0; i < 7; i++)
		{
			float textureWidth = static_cast<float>(bonusImage[i]->GetTextureWidth());
			float textureHeight = static_cast<float>(bonusImage[i]->GetTextureHeight());

			bonusImageFrame[i]->RenderCenter(dc,
				bonusImagePosX[i], screenHeight * 0.5,
				screenWidth * 0.77f, screenHeight * 0.78f,
				0, 0,
				textureWidth, textureHeight,
				0,
				bonusImageFrameColor.x, bonusImageFrameColor.y, bonusImageFrameColor.z, bonusImageOpacity);

			bonusImage[i]->RenderCenter(dc,
				bonusImagePosX[i], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				bonusImageColor[i], bonusImageColor[i], bonusImageColor[i], bonusImageOpacity);
		}

		float textureWidth = static_cast<float>(hint[0]->GetTextureWidth());
		float textureHeight = static_cast<float>(hint[0]->GetTextureHeight());

		//! おにごっこ
		if (!clear.onigokko)
		{
#if 1
			hint[0]->RenderCenter(dc,
				bonusImagePosX[0], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, bonusImageOpacity);
#endif
		}
		//! だるまさんが転んだ
		if (!clear.darumasangaKoronda)
		{
#if 1
			hint[1]->RenderCenter(dc,
				bonusImagePosX[1], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, bonusImageOpacity);
#endif
		}
		//! 寸止め
		if (!clear.sundome)
		{
#if 1
			hint[2]->RenderCenter(dc,
				bonusImagePosX[2], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, bonusImageOpacity);
#endif
		}
		//! 空飛ぶ風船割り
		if (!clear.soratobuHusenWari)
		{
#if 1
			hint[3]->RenderCenter(dc,
				bonusImagePosX[3], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, bonusImageOpacity);
#endif
		}
		//! おっさん叩き
		if (!clear.ossanTataki)
		{
#if 1
			hint[4]->RenderCenter(dc,
				bonusImagePosX[4], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, bonusImageOpacity);
#endif
		}
		//! 足場渡り
		if (!clear.asibawatari)
		{
#if 1
			hint[5]->RenderCenter(dc,
				bonusImagePosX[5], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, bonusImageOpacity);
#endif
		}

		if (clear.onigokko
		 && clear.darumasangaKoronda
		 && clear.sundome
		 && clear.soratobuHusenWari
		 && clear.ossanTataki
		 && clear.asibawatari)
			gameComplete = true;

		if (!gameComplete)
		{
			hint[6]->RenderCenter(dc,
				bonusImagePosX[6], screenHeight * 0.5f,
				screenWidth * 0.75f, screenHeight * 0.75f,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, bonusImageOpacity);
		}
	}
	
#ifndef _DEBUG

	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::ColorEdit3("BonusImageFrameColor", &bonusImageFrameColor.x);
			ImGui::InputFloat3("BonusImageFrameColorNum", &bonusImageFrameColor.x);

			for (int i = 0; i < 6; i++)
			{
				std::string name = "BonusImagePosX" + std::to_string(i + 1);
				ImGui::DragFloat(name.c_str(), &bonusImagePosX[i]);
			}

			for(int i = 0; i < 6; i++)
			{
				std::string name = "GameSelectSpritePos" + std::to_string(i + 1);
				ImGui::DragFloat2(name.c_str(), &gameSelectSpritePos.x);
			}
			ImGui::Image(shadowMap.GetSRV(), ImVec2(300, 200));
			StageManager::Instance().DrawDebugGUI();

			GameSelectManager::Instance().DrawDebugGUI();

			// プレイヤーデバッグ描画
			player->DrawDebugGUI();
//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
//-----------------------------------------------------------------------------------------------------//
			// !カメラのデバッグ描画
			if (ImGui::TreeNode("Cameras"))
			{
				//-------------------------------------------------------------------------------------------------------
				ImGui::Spacing(); // 一行空ける
				ImGui::Separator(); // セクションの間に区切り線を表示
				ImGui::Spacing(); // 一行空ける
				//-------------------------------------------------------------------------------------------------------
				Camera::Instance().DrawDebugGUI(); // ! カメラ

				ImGui::Spacing(); // 一行空ける
				ImGui::Separator(); // セクションの間に区切り線を表示
				ImGui::Spacing(); // 一行空ける

				cameraController->DrawDebugGUI(); // !カメラコントローラー
				ImGui::TreePop();
			}
//-----------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
//-----------------------------------------------------------------------------------------------------//
			EnemyManager::Instance().DrawDebugGUI();
		}
		ImGui::End();
	}
#endif
}

// プレイヤーの位置制限
void SceneGameSelect::PlayerPositionControll()
{
	float posX = 185.0f;

	if (player->GetPosition().x < -posX || player->GetPosition().x > posX)
	{
		player->SetVelocityX(0);
		float positoinX = std::clamp(player->GetPosition().x, -posX, posX);
		player->SetPositionX(positoinX);
	}

	if (player->GetPosition().z < -61.2 || player->GetPosition().z > 130.0f)
	{
		player->SetVelocityZ(0);
		float positoinZ = std::clamp(player->GetPosition().z, -61.2f, 130.0f);
		player->SetPositionZ(positoinZ);
	}
}

// ゲーム画面描画
void SceneGameSelect::RenderGameSprite(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	GameSelectManager& gameSelectManager = GameSelectManager::Instance();
	int gameSelectCount = gameSelectManager.GetGameSelectCount();

	for (int i = 0; i < gameSelectCount; i++)
	{
		std::unique_ptr<GameSelect>& gameSelect = gameSelectManager.GetGameSelect(i);

		DirectX::XMFLOAT3 gameSelectPos = gameSelect->GetPosition();

		DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&gameSelectPos);
		DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project(
			Pos,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World);

		// スクリーン座標を取得
		DirectX::XMFLOAT3 screenPos;
		DirectX::XMStoreFloat3(&screenPos, ScreenPos);

		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Unproject(
			ScreenPos,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMFLOAT3 worldPos;
		DirectX::XMStoreFloat3(&worldPos, WorldPosition);

		Graphics& graphics = Graphics::Instance();

		float screenWidth  = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(gameSelectSprite[i]->GetTextureWidth());
		float textureHeight = static_cast<float>(gameSelectSprite[i]->GetTextureHeight());

		gameSelectSpritePos.x = screenPos.x - (gameSelect->GetWidth() * 8.0f);
		gameSelectSpritePos.y = screenPos.y - (gameSelect->GetHeight() * 6.0f);

		//! ゲーム選択画面
		gameSelectSprite[i]->Render(dc,
			gameSelectSpritePos.x, gameSelectSpritePos.y,
			screenWidth * 0.27f, screenHeight * 0.27f,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 0.5f);
	}
}

// ボーナス画像の表示処理
void SceneGameSelect::UpdateBonusImage(float elapsedTime)
{
	if (clear.onigokko)           bonusImageColor[0] = 1;
	if (clear.darumasangaKoronda) bonusImageColor[1] = 1;
	if (clear.sundome)            bonusImageColor[2] = 1;
	if (clear.soratobuHusenWari)  bonusImageColor[3] = 1;
	if (clear.ossanTataki)        bonusImageColor[4] = 1;
	if (clear.asibawatari)        bonusImageColor[5] = 1;
	if (gameComplete)             bonusImageColor[6] = 1;

	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_START && !fade->GetFade())
	{
		viewBonusImage = !viewBonusImage;

		float startFade = viewBonusImage ? 0.0f : 1.0f;
		float endFade = viewBonusImage   ? 1.0f : 0.0f;

		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			startFade, endFade,
			0.5f);
	}

	//! 表示しないなら
	if (!viewBonusImage) 
	{
		if (bonusImageOpacity > 0.0f)
			bonusImageOpacity -= 3 * elapsedTime;
		else
			bonusImageOpacity = 0.0f;

		return;
	}

	if (bonusImageOpacity < 1.0f)
		bonusImageOpacity += 3 * elapsedTime;
	else
		bonusImageOpacity = 1.0f;

	if (inputWaitTime > 0.0f) inputWaitTime -= elapsedTime;

	if ((gamePad.GetButtonDown() || gamePad.GetButtonHeld()) && !bonusImageMove && inputWaitTime <= 0.0f && !fade->GetFade())
	{
		int button = gamePad.GetButtonDown() | gamePad.GetButtonHeld();

		float screenWidth = static_cast<float>(Graphics::Instance().GetScreenWidth());

		switch (button)
		{
		case GamePad::BTN_LEFT:
		{
			if (bonusImageNum <= 1) return;

			for (int i = 0; i < 7; i++)
			{
				startBonusImagePosX[i] = bonusImagePosX[i];
				endBonusImagePosX[i] = bonusImagePosX[i] + screenWidth;
			}

			bonusImageNum--;
			bonusImageMove = true;
		}
		break;
		case GamePad::BTN_RIGHT:
		{
			if (bonusImageNum >= 7) return;

			for (int i = 0; i < 7; i++)
			{
				startBonusImagePosX[i] = bonusImagePosX[i];
				endBonusImagePosX[i] = bonusImagePosX[i] - screenWidth;
			}

			bonusImageNum++;
			bonusImageMove = true;
		}
		break;
		default:
		{
			if (button != GamePad::BTN_RIGHT && button != GamePad::BTN_LEFT) return;

			for (int i = 0; i < 7; i++)
			{
				bonusImagePosX[i] = endBonusImagePosX[i];
			}
		}
			break;
		}
	}

	if (bonusImageMove)
	{
		bonusImageElapsedTime += elapsedTime;
		float t = bonusImageElapsedTime / 0.8f;

		if (t < 1.0f)
		{
			for (int i = 0; i < 7; i++)
			{
				bonusImagePosX[i] = Easing::EaseOut(startBonusImagePosX[i], endBonusImagePosX[i], t);
			}
		}
		else
		{
			bonusImageElapsedTime = 0.0f;
			inputWaitTime = 0.1f;
			bonusImageMove = false;
		}
	}
	else return;
}