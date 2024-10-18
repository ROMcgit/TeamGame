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
#include "Game/Character/Item/Melon.h"
#include "Game/Character/Item/Peach.h"
#include "Game/Character/Item/Mikan.h"
#include "Game/Character/Item/Banana.h"

#include "Game/Character/Installation/InstallationManager.h"
#include "Game/Effect/EffectManager.h"
#include "Input/Input.h"
#include "SceneTitle.h"

#include "Game/Stage/StageMain.h"
#include "Game/Stage/StageManager.h"

#include "Audio/BgmManager.h"
#include "Audio/SoundEffectManager.h"

// 初期化
void SceneTutorialAction::Initialize()
{
	for (int i = 0; i < 6; i++)
	{
		std::string filePath = "Data/Sprite/チュートリアル/メッセージ/メッセージ" + std::to_string(i + 1) + ".png";
		message[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	text = std::make_unique<Text>();

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

	//! 音楽読み込み
	BgmManager::Instance().LoadBgm("トレーニング", "Data/Audio/bgm/training.wav");
	BgmManager::Instance().PlayBgm("トレーニング", 0.3f);
	BgmManager::Instance().LoadBgm("無敵", "Data/Audio/bgm/無敵.wav");

	SoundEffectManager::Instance().LoadSoundEffect("正解", "Data/Audio/正解.wav");
}

// 終了化
void SceneTutorialAction::Finalize()
{
	// ステージ終了化
	StageManager::Instance().Clear();

	// エネミー終了化
	/*EnemyManager::Instance().Clear();*/

	// アイテム終了化
	ItemManager::Instance().Clear();

	// 設置物終了化
	/*InstallationManager::Instance().Clear();*/
}

// 更新処理
void SceneTutorialAction::Update(float elapsedTime)
{
	if (player->GetInvincibleState())
	{
		if (!mutekiBgmPlay)
		{
			BgmManager::Instance().PlayBgm("無敵", 0.4f);
			mutekiBgmPlay = true;
		}

		BgmManager::Instance().StopBgm("トレーニング");
	}
	else
	{
		BgmManager::Instance().StopBgm("無敵");
		if (mutekiBgmPlay)
		{
			mutekiBgmPlay = false;
			BgmManager::Instance().PlayBgm("トレーニング", 0.3f);
		}
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	switch (messageScene)
	{
	case MessageScene::Message1:
	{
		messageNum = 1;

		if (messageOpacity >= 1.0f && !messageFinish[0])
		{
			answerTimer = 1.0f;
			messageFinish[0] = true;
		}
		
		if (messageFinish[0] && answerTimer < 0.0f)
		{
			messageOpacity -= 0.8f * elapsedTime;
			messageOpacityWaitTime = 1.0f;
		}

		//! 次のメッセージへ
		if (messageOpacity <= 0.0f && messageFinish[0])
		{
			messageScene = MessageScene::Message2;
		}
	}
		break;
	case MessageScene::Message2:
	{
		messageNum = 2;

		textPos = { 1032, 637 };

		if (messageOpacity >= 1.0f && 
			(gamePad.GetButtonHeld() & GamePad::BTN_LEFT || gamePad.GetButtonHeld() & GamePad::BTN_RIGHT))
		{
			inputTimer += elapsedTime;
		}

		if (inputTimer >= 3 && !messageFinish[1])
		{
			SoundEffectManager::Instance().PlaySoundEffect("正解");
			answerTimer = 1.0f;
			messageFinish[1] = true;
		}

		if (messageFinish[1] && answerTimer < 0.0f)
		{
			messageOpacity -= 0.8f * elapsedTime;
			messageOpacityWaitTime = 1.0f;
		}

		if (messageFinish[1] && messageOpacity <= 0.0f)
		{
			messageScene = MessageScene::Message3;
		}
	}
		break;
	case MessageScene::Message3:
	{
		messageNum = 3;

		textPos = { 845, 637 };

		if (messageOpacity >= 1.0f &&
			(gamePad.GetButtonDown() & GamePad::BTN_B || gamePad.GetButtonDown() & GamePad::BTN_X))
		{
			inputCount++;
		}

		if (inputCount >= 20 && !messageFinish[2])
		{
			SoundEffectManager::Instance().PlaySoundEffect("正解");
			answerTimer = 1.0f;
			messageFinish[2] = true;
		}

		if (answerTimer <= 0.0f && messageFinish[2])
		{
			messageOpacity -= 0.8f * elapsedTime;
			messageOpacityWaitTime = 1.0f;
		}

		if (messageFinish[2] && messageOpacity <= 0.0f)
		{
			messageScene = MessageScene::Message4;
		}
	}
		break;
	case MessageScene::Message4:
	{
		messageNum = 4;

		textPos = { 930, 637 };

		if (messageOpacity >= 1.0f &&
			player->GetLungesCount() >= 3 && !messageFinish[3])
		{
			SoundEffectManager::Instance().PlaySoundEffect("正解");
			answerTimer = 1.0f;
			messageFinish[3] = true;
		}
			
		if (answerTimer <= 0.0f && messageFinish[3])
		{
			messageOpacity -= 0.8f * elapsedTime;
			messageOpacityWaitTime = 1.0f;
		}

		if (messageFinish[3] && messageOpacity <= 0.0f)
		{
			messageScene = MessageScene::Message5;
		}
	}
		break;
	case MessageScene::Message5:
	{
		messageNum = 5;

		if (messageOpacity >= 1.0f && !messageFinish[4])
		{
			answerTimer = 2.0f;
			messageFinish[4] = true;
		}

		if (messageFinish[4] && answerTimer < 0.0f)
		{
			messageOpacity -= 0.8f * elapsedTime;
			messageOpacityWaitTime = 1.0f;
		}

		//! 次のメッセージへ
		if (messageOpacity <= 0.0f && messageFinish[4])
		{
			messageScene = MessageScene::Message6;
		}
	}
		break;
	case MessageScene::Message6:
	{
		messageNum = 6;

		if (gamePad.GetButtonDown() & GamePad::BTN_Y && !setFade)
		{
			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				0.0f, 1.0f,
				4.0f
			);

			setFade = true;
		}

		if (setFade && !fade->GetFade())
		{
			std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGame>());
			// シーンマネージャーにローディングシーンへの切り替えを指示
			SceneManager::Instance().ChangeScene(std::move(loadingScene));
		}
	}
		break;
	default:
		break;
	}

	if (messageScene != MessageScene::Message4)
		player->SetLungesCount(0);

	if (answerTimer > 0.0f)
		answerTimer -= elapsedTime;

	if (messageOpacityWaitTime > 0.0f)
		messageOpacityWaitTime -= elapsedTime;

	//! メッセージの不透明度
	if (messageOpacityWaitTime <= 0.0 && messageOpacity < 1.0f)
		messageOpacity += elapsedTime;

	// 生成処理
	Newestablishment(elapsedTime);

	// カメラコントローラー更新処理
	if (!cameraController->GetCameraMovie())
	{
		target = player->GetPosition();
		target.y += 0.8f;
	}

	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// アイテム更新処理
	ItemManager::Instance().Update(elapsedTime);

	// 重要アイテム更新処理
	ImportantItemManager::Instance().Update(elapsedTime);

	// 設置物更新処理
	InstallationManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	fade->Update(elapsedTime);
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

		SceneTitle& scene = SceneTitle::Instance();

		StageManager::Instance().Render(dc, shader);

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

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	//! 2D画像
	{
		player->SpriteRender(dc);

		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());

		float textureWidth = static_cast<float>(message[messageNum - 1]->GetTextureWidth());
		float textureHeight = static_cast<float>(message[messageNum - 1]->GetTextureHeight());

		message[messageNum - 1]->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, messageOpacity);

		switch (messageNum)
		{
		case 2:
			text->Render(dc,
				true, true, false, false, false,
				0, 0, 0, (int)inputTimer,
				textPos.x, textPos.y,
				textScale.x, textScale.y,
				0,
				10,
				1, 1, 1, messageOpacity);
			break;
		case 3:
			text->Render(dc,
				true, true, false, false, false,
				0, 0, 0, inputCount,
				textPos.x, textPos.y,
				textScale.x, textScale.y,
				0,
				42,
				1, 1, 1, messageOpacity);
			break;
		case 4:
			text->Render(dc,
				true, true, false, false, false,
				0, 0, 0, player->GetLungesCount(),
				textPos.x, textPos.y,
				textScale.x, textScale.y,
				0,
				10,
				1, 1, 1, messageOpacity);
			break;
		default:
			break;
		}
		
	}

#ifdef _DEBUG
	// 2DデバッグGUI描画
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::DragFloat2("TextPos", &textPos.x);
			ImGui::DragFloat2("TextScale", &textScale.x, 0.1f);
		}
		ImGui::End();
	}
#endif

	fade->Render(dc);
}

// 生成処理
void SceneTutorialAction::Newestablishment(float elapsedTime)
{
	ItemManager& itemManager = ItemManager::Instance();

	int itemCount = itemManager.GetItemCount();                 // アイテムの数

	if (player->GetPosition().y > 60)
	{
		for (int i = 0; i < itemCount; i++)
		{
			std::unique_ptr<Item>& item = itemManager.GetItem(i);
			item->Destroy();
		}
	}

/***************************************************************************************************/

	if (player->GetIsGround() && !cameraController->GetCameraMovie())
	{
		float angleY = player->GetAngle().y;

		// 生成する範囲の角度をランダムに決める
		float randomAngle = angleY + (rand() % 360 - 180) * (3.14159f / 180.0f); // -180度から+180度までランダム

		float distance = rand() % 20 + 20; // プレイヤーからの距離もランダム

		int posX = player->GetPosition().x + distance * cos(randomAngle); // cosでX座標を計算
		int posZ = player->GetPosition().z + distance * sin(randomAngle); // sinでZ座標を計算

		if (itemCount < 15 && newItemTimer > newItemMaxTimer && messageFinish[4])
		{
			posX = player->GetPosition().x + distance * cos(randomAngle);
			posZ = player->GetPosition().z + distance * sin(randomAngle);

			//! 位置制限(X)
			if (posX >= 990)
				posX = 950;
			else if (posX <= -990)
				posX = -950;

			//! 位置制限(Z)
			if (posZ >= 990)
				posZ = 950;
			else if (posZ <= -990)
				posZ = -950;

			int ItemRand = rand() % 20 + 1;
			switch (ItemRand)
			{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			{
				std::unique_ptr<Apple> apple = std::make_unique<Apple>();
				apple->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(apple));
			}
				break;
			case 9:
			case 10:
			{
				std::unique_ptr<Melon> melon = std::make_unique<Melon>();
				melon->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(melon));
			}
				break;
			case 11:
			case 12:
			{
				std::unique_ptr<Peach> peach = std::make_unique<Peach>();
				peach->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(peach));
			}
				break;
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			{
				std::unique_ptr<Mikan> mikan = std::make_unique<Mikan>();
				mikan->SetPosition(DirectX::XMFLOAT3(posX, 1, posZ));
				itemManager.Register(std::move(mikan));
			}
			default:
				break;
			}

			newItemTimer = 0.0f;
		}
		newItemTimer += elapsedTime;
	}
}