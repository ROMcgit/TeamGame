#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Collision.h"
#include "Input/Input.h"
#include "Game/Character/Player.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Scene/SceneTitle.h"
#include "Game/Character/Projectile/ProjectileUnko.h"
#include "Game/Character/Yazirusi/YazirusiPlayer.h"
#include "Game/Character/Item/ImportantItem.h"
#include "Game/Character/Item/ImportantItemManager.h"
#include "Audio/SoundEffectManager.h"

static Player* instance = nullptr;

// インスタンス取得
Player& Player::Instance()
{
	return *instance;
}

Player::Player()
{
	// インスタンスポインタ設定
	instance = this;

	model[0] = std::make_unique <Model>("Data/Model/Monkey/0.mdl/Monkey_Face.mdl");
	model[1] = std::make_unique <Model>("Data/Model/Monkey/0.mdl/Monkey_Leg_Left.mdl");
	model[2] = std::make_unique <Model>("Data/Model/Monkey/0.mdl/Monkey_Leg_Right.mdl");

	YazirusiPlayer* yazirusi = new YazirusiPlayer(&yazirusiManager);

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	ui[0] = std::make_unique<Sprite>("Data/Sprite/UI/Lv.png");
	ui[1] = std::make_unique<Sprite>("Data/Sprite/UI/Speed.png");
	ui[2] = std::make_unique<Sprite>("Data/Sprite/UI/cm.png");
	ui[3] = std::make_unique<Sprite>("Data/Sprite/UI/ハテナ.png");
	ui[4] = std::make_unique<Sprite>("Data/Sprite/UI/バナナ.png");
	ui[5] = std::make_unique<Sprite>("Data/Sprite/UI/リンゴ最大数.png");

	itemImage[0] = std::make_unique<Sprite>("Data/Sprite/UI/リンゴ.png");

	for (int i = 0; i < 3; i++)
	{
		itemNum[i] = std::make_unique<Text>();
	}

	// 当たり判定
	radius = 0.9f;
	height = 2.4f;
	collisionOffset = { 0, -1.0f, 0 };

	// エフェクト読み込み
	hitEffect        = std::make_unique<Effect>("Data/Effect/Effect/Hit.efk");
	sppedEffect      = std::make_unique<Effect>("Data/Effect/Effect/Speed.efk");
	invincibleEffect = std::make_unique<Effect>("Data/Effect/Effect/Invincibility.efk");

	// 移動ステートへ遷移
	TransitionMoveState();

	//! HP設定
	hp = hpDamage = maxHp = 300;

	//! HPの長さ調整
	hpSpriteAdjust = 0.615f;

	//! HP画像位置
	hpImagePos = { 30, 660 };
	hpImageShakePosY = hpImagePos.y;

	//! HPゲージの位置
	hpSpritePos = { 38, 668.3f };
	hpSpriteShakePosY = hpSpritePos.y;

	//! HP
	hpSprite[0] = std::make_unique<Sprite>("Data/Sprite/UI/HP.png");

	for (int i = 0; i < 3; i++)
	{
		hpSprite[i + 1] = std::make_unique<Sprite>();
	}

	//! 体当たり
	runAttack[0] = std::make_unique<Sprite>("Data/Sprite/UI/HP.png");

	for (int i = 0; i < 2; i++)
	{
		runAttack[i + 1] = std::make_unique<Sprite>();
	}

	for(int i = 0; i < 4; i++)
		text[i] = std::make_unique<Text>();

	SoundEffectManager& sound = SoundEffectManager::Instance();
	sound.LoadSoundEffect("うんこ", "Data/Audio/うんこ.wav");
	sound.LoadSoundEffect("叫び", "Data/Audio/voice/叫び.wav");
	sound.LoadSoundEffect("攻撃ヒット", "Data/Audio/攻撃ヒット.wav");
	sound.LoadSoundEffect("突進チャージ", "Data/Audio/突進チャージ.wav");
	sound.LoadSoundEffect("突進ゲージマックス", "Data/Audio/ゲージマックス.wav");
	sound.LoadSoundEffect("突進", "Data/Audio/突進.wav");
}

// デストラクタ
Player::~Player()
{
}

// 更新処理
void Player::Update(float elapsedTime)
{
	if (position.y > 1.3f)
	{
		isGround = false;
		gravity = -1.0f;
	}

	if (position.y < 1.3f)
	{
		isGround   = true;
		gravity    = 0.0f;
		position.y = 1.3f;
	}

	/// X座標制限
	if (position.x > 1000.0f)
		position.x = 1000.0f;
	else if (position.x < -1000.0f)
		position.x = -1000.0f;

	// Z座標制限
	if (position.z > 1000.0f)
		position.z = 1000.0f;
	else if (position.z < -1000.0f)
		position.z = -1000.0f;

	if (viewMoveSpeed > 0)
	{
		//! カラーX
		if (!viewMoveSpeedColorUp.x)
		{
			viewMoveSpeedColor.x -= viewMoveSpeedColorSpeed.x * elapsedTime;

			if (viewMoveSpeedColor.x < 0.0f)
			{
				viewMoveSpeedColor.x   = 0.0f;
				viewMoveSpeedColorUp.x = true;
			}
		}
		else
		{
			viewMoveSpeedColor.x += viewMoveSpeedColorSpeed.x * elapsedTime;

			if (viewMoveSpeedColor.x > 1.0f)
			{
				viewMoveSpeedColor.x   = 1.0f;
				viewMoveSpeedColorUp.x = false;
			}
		}

		//! カラーY
		if (!viewMoveSpeedColorUp.y)
		{
			viewMoveSpeedColor.y -= viewMoveSpeedColorSpeed.y * elapsedTime;

			if (viewMoveSpeedColor.y < 0.0f)
			{
				viewMoveSpeedColor.y   = 0.0f;
				viewMoveSpeedColorUp.y = true;
			}
		}
		else
		{
			viewMoveSpeedColor.y += viewMoveSpeedColorSpeed.y * elapsedTime;

			if (viewMoveSpeedColor.y > 1.0f)
			{
				viewMoveSpeedColor.y   = 1.0f;
				viewMoveSpeedColorUp.y = false;
			}
		}

		//! カラーZ
		if (!viewMoveSpeedColorUp.z)
		{
			viewMoveSpeedColor.z -= viewMoveSpeedColorSpeed.z * elapsedTime;

			if (viewMoveSpeedColor.z < 0.0f)
			{
				viewMoveSpeedColor.z   = 0.0f;
				viewMoveSpeedColorUp.z = true;
			}
		}
		else
		{
			viewMoveSpeedColor.z += viewMoveSpeedColorSpeed.z * elapsedTime;

			if (viewMoveSpeedColor.z > 1.0f)
			{
				viewMoveSpeedColor.z   = 1.0f;
				viewMoveSpeedColorUp.z = false;
			}
		}
	}
	else
	{
		if (viewMoveSpeedColor.x < 1.0f)
			viewMoveSpeedColor.x += 0.5f * elapsedTime;

		if(viewMoveSpeedColor.y < 1.0f)
			viewMoveSpeedColor.y += 0.5f * elapsedTime;

		if (viewMoveSpeedColor.z < 1.0f)
			viewMoveSpeedColor.z += 0.5f * elapsedTime;
	}

	// ムービー中なら待機ステートへ遷移
	if (movieScene)
	{
		// ムービー中のアニメーション
		if (!movieAnimation)
		{
			state = State::Lunges; // ステートを待機に変更
			for(int i = 0; i < 3; i++)
			model[i]->PlayAnimation(movieAnimNum, movieAnimLoop);
			movieAnimation = true;
		}
	}
	// ムービー中では無い時
	else
		movieAnimation = false; // ムービー中に待機ステートかどうか

	if (!movieScene)
	{
		// ステート毎の処理
		switch (state)
		{
		case State::Move:
			UpdateMoveState(elapsedTime);
			break;
		case State::Lunges:
			UpdateLungesState(elapsedTime);
			break;
		case State::Attack:
			UpdateAttackState(elapsedTime);
			break;
		case State::Damage:
			UpdateDamageState(elapsedTime);
			break;
		case State::Death:
			UpdateDeathState(elapsedTime);
			break;
		}
	}

	// 速力更新処理
	UpdateVelocity(elapsedTime);

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// 矢印更新処理
	yazirusiManager.Update(elapsedTime);

	// HP管理
	HpControll(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// 弾丸と敵に衝突処理
	CollisionProjectilesVsEnemies();

	// レベル更新処理
	UpdateLevel();

	// 当たり判定の位置設定
	CollisionPosSettings();

	// オブジェクト行列を更新
	UpdateTransform();

	// ムービー時間タイマー
	UpdateMovieTimer(elapsedTime);

	// モデルアニメーション更新処理
	for (int i = 0; i < 3; i++)
	{
		model[i]->UpdateAnimation(elapsedTime);

		// モデル更新処理
		model[i]->UpdateTransform(transform);
	}

	viewMoveSpeedPlusTimer += elapsedTime;

	DirectX::XMFLOAT3 pos = position;
	pos.y += height * 0.5f;
	sppedEffect->SetPosition(speedHandle, pos);

	if (invincibleState)
	{
		if (!invincibleEffectPlay)
		{
			invincibleHandle     = invincibleEffect->Play(pos, 0.5f);
			invincibleEffectPlay = true;
		}
		
		invincibleEffect->SetPosition(invincibleHandle, pos);

		if (invincibleTimer <= 0.0f)
		{
			invincibleEffect->Stop(invincibleHandle);

			invincibleState      = false;
			invincibleEffectPlay = false;
		}
			
	}
}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (int i = 0; i < 3; i++)
	shader->Draw(dc, model[i].get());

	// 弾丸描画処理
	projectileManager.Render(dc, shader);

	// 矢印描画処理
	yazirusiManager.Render(dc, shader);
}

// HPなどのUI描画
void Player::SpriteRender(ID3D11DeviceContext* dc)
{
	float textureWidth = static_cast<float>(hpSprite[1]->GetTextureWidth());
	float textureHeight = static_cast<float>(hpSprite[1]->GetTextureHeight());

	if (!hideSprites)
	{
		// ゲージの裏(黒色)
		hpSprite[1]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			185, 30,
			0, 0, textureWidth, textureHeight,
			0,
			0.0f, 0.0f, 0.0f, 1);

		if (hp > 25)
		{
			// ダメージ(赤色)
			hpSprite[2]->Render(dc,
				hpSpritePos.x , hpSpritePos.y,
				hpDamage * hpSpriteAdjust, 28,
				0, 0, textureWidth, textureHeight,
				0,
				1, 0, 0, 1);

			// ダメージ(緑色)
			hpSprite[3]->Render(dc,
				hpSpritePos.x, hpSpritePos.y,
				hp * hpSpriteAdjust, 28,
				0, 0, textureWidth, textureHeight,
				0,
				0, 1, 0, 1);
		}
		else
		{
			textureWidth  = static_cast<float>(hpSprite[1]->GetTextureWidth());
			textureHeight = static_cast<float>(hpSprite[1]->GetTextureHeight());

			// ダメージ(白色)
			hpSprite[2]->Render(dc,
				hpSpritePos.x, hpSpritePos.y,
				hpDamage * 1.34f, 25,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);

			// ダメージ(赤色)
			hpSprite[3]->Render(dc,
				hpSpritePos.x, hpSpritePos.y,
				hp * 1.34f, 25,
				0, 0, textureWidth, textureHeight,
				0,
				1, 0, 0, 1);
		}

		textureWidth = static_cast<float>(itemImage[0]->GetTextureWidth());
		textureHeight = static_cast<float>(itemImage[0]->GetTextureHeight());

		//! リンゴ
		itemImage[0]->Render(dc,
			240, 640,
			135 * 0.4f, 169 * 0.4f,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		//! リンゴの数
		itemNum[0]->Render(dc,
			true, false, false, false, false,
			0, 0, 0, diffusionAttacks,
			295, 675,
			6, 6,
			0,
			0,
			1, 1, 1, 1);

		textureWidth = static_cast<float>(ui[5]->GetTextureWidth());
		textureHeight = static_cast<float>(ui[5]->GetTextureHeight());

		ui[5]->Render(dc,
			0, 0,
			1280, 720,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		textureWidth  = static_cast<float>(hpSprite[0]->GetTextureWidth());
		textureHeight = static_cast<float>(hpSprite[0]->GetTextureHeight());	
		//! HP画像
		hpSprite[0]->Render(dc,
			hpImagePos.x, hpImagePos.y,
			200, 44,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1
		);

		{
			textureWidth = static_cast<float>(runAttack[0]->GetTextureWidth());
			textureHeight = static_cast<float>(runAttack[0]->GetTextureHeight());

			// ゲージの裏(黒色)
			runAttack[1]->Render(dc,
				hpSpritePos.x + hpSpriteMinus.x - 5, hpSpritePos.y + hpSpriteMinus.y - 3,
				140, 26,
				0, 0, textureWidth, textureHeight,
				0,
				0.0f, 0.0f, 0.0f, 1);

			if (lungesChargeTimer < 0.6f)
			{
				// ゲージの色(緑色)
				runAttack[2]->Render(dc,
					hpSpritePos.x + hpSpriteMinus.x - 5, hpSpritePos.y + hpSpriteMinus.y - 3,
					lungesChargeTimer * 240, 26,
					0, 0, textureWidth, textureHeight,
					0,
					0.0f, 0.0f, 1.0f, 1);
			}
			else
			{
				// ゲージの色(緑色)
				runAttack[2]->Render(dc,
					hpSpritePos.x + hpSpriteMinus.x - 5, hpSpritePos.y + hpSpriteMinus.y - 3,
					lungesChargeTimer * 240, 26,
					0, 0, textureWidth, textureHeight,
					0,
					0.0f, 1.0f, 1.0f, 1);
			}

			//! HP画像
			runAttack[0]->Render(dc,
				hpImagePos.x + hpSpriteMinus.x, hpImagePos.y + hpSpriteMinus.y,
				150, 34,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1
			);
		}

		ImportantItemManager& importantItemManager = ImportantItemManager::Instance();
		int importantItemCount = importantItemManager.GetImportantItemCount();

		textureWidth = static_cast<float>(ui[2]->GetTextureWidth());
		textureHeight = static_cast<float>(ui[2]->GetTextureHeight());

		//! cm
		ui[2]->Render(dc,
			240, 15,
			76, 36,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1
		);

		textureWidth = static_cast<float>(ui[4]->GetTextureWidth());
		textureHeight = static_cast<float>(ui[4]->GetTextureHeight());

		//! バナナ
		ui[4]->Render(dc,
			325, 5,
			60, 48,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		//! バナナの数
		text[3]->Render(dc,
			true, false, false, false, false,
			0, 0, 0, bananaNum,
			380, 22,
			6, 6,
			0,
			0,
			1, 1, 1, 1);

		if (importantItemCount > 0)
		{
			std::unique_ptr<ImportantItem>& banana = importantItemManager.GetImportantItem(0);

			text[0]->Render(dc,
				true, true,
				false,
				0, 0, 0, 0, 0, 0,
				0, 0, banana->GetDist(),
				-50, 5,
				10, 10,
				0,
				30,
				1, 1, 1, 1);
		}
		else
		{
			textureWidth = static_cast<float>(ui[3]->GetTextureWidth());
			textureHeight = static_cast<float>(ui[3]->GetTextureHeight());

			//! ？？？？
			ui[3]->Render(dc,
				12, 5,
				216, 48,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}

		textureWidth  = static_cast<float>(ui[0]->GetTextureWidth());
		textureHeight = static_cast<float>(ui[0]->GetTextureHeight());

		// レベル文字
		ui[0]->Render(dc,
			20, 60,
			82, 50,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 0, 1);

		if (level < 10)
		{
			// レベル
			text[1]->Render(dc,
				true, true,
				false, false, false,
				0, 0, 0, level,
				15, 60,
				10, 10,
				0,
				30,
				1, 1, 1, 1);
		}
		else if(level < 20)
		{
			// レベル
			text[1]->Render(dc,
				true, true,
				false, false, false,
				0, 0, 0, level,
				35, 60,
				10, 10,
				0,
				30,
				1, 1, 1, 1);
		}
		else
		{
			// レベル
			text[1]->Render(dc,
				true, true,
				false, false, false,
				0, 0, 0, level,
				35, 60,
				10, 10,
				0,
				30,
				1, 1, 0, 1);
		}
		

		textureWidth = static_cast<float>(ui[1]->GetTextureWidth());
		textureHeight = static_cast<float>(ui[1]->GetTextureHeight());

		// スピード文字
		ui[1]->Render(dc,
			200, 60,
			176, 50,
			0, 0, textureWidth, textureHeight,
			0,
			0, 1, 1, 1);

		// スピード
		if (viewMoveSpeed < (int)moveSpeed + 0.5f && viewMoveSpeedPlusTimer > 0.05f)
		{
			viewMoveSpeedPlusTimer = 0.0f;
			viewMoveSpeed++;
		}
		else if (viewMoveSpeed > (int)moveSpeed + 0.5f)
			viewMoveSpeed = (int)moveSpeed + 0.5f;

		if (viewMoveSpeed < 10)
		{
			// スピード
			text[2]->Render(dc,
				true, true,
				false, false, false,
				0, 0, 0, viewMoveSpeed,
				298, 60,
				10, 10,
				0,
				30,
				1, 1, 1, 1);
		}
		else
		{
			// スピード
			text[2]->Render(dc,
				true, true,
				false, false, false,
				0, 0, 0, viewMoveSpeed,
				320, 60,
				10, 10,
				0,
				35,
				viewMoveSpeedColor.x,
				viewMoveSpeedColor.y,
				viewMoveSpeedColor.z,
				1);
		}
	}
}

// レベル更新処理
void Player::UpdateLevel()
{
	switch (level)
	{
	case 1:
		expMax = 5;
		break;
	case 2:
		expMax = 10;
		break;
	case 3:
		expMax = 12;
		break;
	case 4:
		expMax = 14;
		break;
	case 5:
		expMax = 16;
		break;
	// 敵HP = 2
	case 6:
		expMax = 20;
		break;
	case 7:
		expMax = 22;
		break;
	case 8:
		expMax = 25;
		break;
	case 9:
		expMax = 26;
		break;
	case 10:
		expMax = 30;
		break;
	// 敵HP = 3
	case 11:
		expMax = 14;
		break;
	case 12:
		expMax = 16;
		break;
	case 13:
		expMax = 18;
		break;
	case 14:
		expMax = 20;
		break;
	case 15:
		expMax = 22;
		break;
	// 敵HP = 4
	case 16:
		expMax = 15;
		break;
	case 17:
		expMax = 16;
		break;
	case 18:
		expMax = 17;
		break;
	case 19:
		expMax = 18;
		break;
	case 20:
		expMax = 19;
		break;
	default:
		break;
	}

	if (exp >= expMax && level < 20)
	{
		exp = 0;
		level++;
	}
}

// 移動入力処理
bool Player::InputMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	// 旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return !(moveVec.x == 0.0f && moveVec.z == 0.0f);
}

// 弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	const GamePadButton attackButton = GamePad::BTN_B | GamePad::BTN_X;

	// 直進弾丸発射
	if (gamePad.GetButtonDown() & attackButton) //CキーとXキー
	{
		SoundEffectManager& sound = SoundEffectManager::Instance();
		sound.StopSoundEffect("うんこ");
		sound.PlaySoundEffect("うんこ");

		//! 拡散攻撃
		if (diffusionAttacks > 0)
		{
			for (int i = 0; i < 36; i++)
			{
				DirectX::XMFLOAT3 dir; // 方向

				// 角度を計算 (360度を36で割ってそれぞれの方向へ飛ばす)
				float angle = DirectX::XM_2PI * i / 36.0f;
				dir.x = cosf(angle);
				dir.z = sinf(angle);

				dir.y = 0.0f;

				/// XMVECTOR型に変換して正規化する
				DirectX::XMVECTOR DIR;
				DIR = DirectX::XMLoadFloat3(&dir);
				DIR = DirectX::XMVector3Normalize(DIR);
				DirectX::XMStoreFloat3(&dir, DIR);

				DirectX::XMFLOAT3 pos; // 位置

				// 位置設定
				pos.x = position.x;
				pos.y = position.y;
				pos.z = position.z;

				ProjectileUnko* projectile = new ProjectileUnko(&projectileManager);
				projectile->Launch(dir, pos);
			}
			diffusionAttacks -= 1;
		}
		else
		{
			// 前方向
			DirectX::XMFLOAT3 dir;

			dir.x = transform._31;
			dir.y = transform._32;
			dir.z = transform._33;

			DirectX::XMVECTOR DIR;
			DIR = DirectX::XMLoadFloat3(&dir);
			DIR = DirectX::XMVector3Normalize(DIR);
			DirectX::XMStoreFloat3(&dir, DIR);

			// 発射位置(プレイヤーの腰あたり)
			DirectX::XMFLOAT3 pos;
			pos.x = position.x;
			pos.y = position.y - (height * 0.5f);
			pos.z = position.z;

			//発射
			ProjectileUnko* projectile = new ProjectileUnko(&projectileManager);
			projectile->Launch(dir, pos);
		}
	}
}

// 攻撃入力処理
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}

	return false;
}

// ノードと敵の衝突処理
//void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
//{
//	// ノード取得
//	Model::Node* node = model->FindNode(nodeName);
//
//	// ノードが存在しない場合は処理を終了
//	if (!node) return;
//
//	// ノード位置取得
//	DirectX::XMMATRIX nodeTransform = DirectX::XMLoadFloat4x4(&node->worldTransform);
//	DirectX::XMVECTOR nodePosVector = nodeTransform.r[3];
//	DirectX::XMFLOAT3 nodePosition;
//	DirectX::XMStoreFloat3(&nodePosition, nodePosVector);
//
//	// 敵マネージャーのインスタンスを取得
//	EnemyManager& enemyManager = EnemyManager::Instance();
//
//	// 指定のノードと全ての敵を総当たりで衝突処理
//	int enemyCount = enemyManager.GetEnemyCount();
//	for (int i = 0; i < enemyCount; ++i)
//	{
//		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);
//
//		// 衝突処理
//		DirectX::XMFLOAT3 outPosition;
//		if (Collision::IntersectSphereVsSphere(
//			nodePosition,
//			nodeRadius,
//			enemy->GetPosition(),
//			enemy->GetRadius(),
//			outPosition
//		))
//		{
//			// 押し出しの後の位置設定
//			enemy->SetPosition(outPosition);
//
//			enemy->ApplyDamage(1, 4.5f);
//		}
//	}
//}

// 移動ステートへ遷移
void Player::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	for (int i = 0; i < 3; i++)
	model[i]->PlayAnimation(Anim_Run, true);
}

// 移動ステート更新処理
void Player::UpdateMoveState(float elapsedTime)
{
	// 前方向
	DirectX::XMFLOAT3 dir;

	dir.x = transform._31;
	dir.y = transform._32;
	dir.z = transform._33;

	DirectX::XMVECTOR DIR;
	DIR = DirectX::XMLoadFloat3(&dir);
	DIR = DirectX::XMVector3Normalize(DIR);
	DirectX::XMStoreFloat3(&dir, DIR);

	//! 突進しているなら
	if (lunges && lungesChargeTimer > 0.0f)
	{
		moveSpeed = 2000.0f;

		invincibleTimer = 0.1f;
		lungesChargeTimer -= elapsedTime;
	}
	else
	{
		lunges = false;

		switch (level)
		{
		case 1:
		case 2:
			moveSpeed = 10;
			break;
		case 3:
		case 4:
			moveSpeed = 15;
			break;
		case 5:
		case 6:
			moveSpeed = 17;
			break;
		case 7:
		case 8:
			moveSpeed = 20;
			break;
		case 9:
			moveSpeed = 23;
			break;
		case 10:
			moveSpeed = 25;
			break;
		case 11:
			moveSpeed = 28;
			break;
		case 12:
			moveSpeed = 32;
			break;
		case 13:
			moveSpeed = 35;
			break;
		case 14:
			moveSpeed = 38;
			break;
		case 15:
			moveSpeed = 42;
			break;
		case 16:
			moveSpeed = 46;
			break;
		case 17:
			moveSpeed = 50;
			break;
		case 18:
			moveSpeed = 55;
			break;
		case 19:
			moveSpeed = 60;
			break;
		case 20:
			moveSpeed = 65;
			break;
		default:
			break;
		}
	}

	if (bananaNum >= 6 && moveSpeed > 15.0f && !lunges)
		moveSpeed = 15.0f;

	Move(dir.x, dir.z, moveSpeed);

	GamePad& gamePad = Input::Instance().GetGamePad();

	float ax = gamePad.GetAxisLX();
	// カメラの回転速度
	float speed = turnSpeed * elapsedTime;
	{
		//スティックの入力値に合わせてX軸とY軸を回転
		if (ax < 0)
		{
			angle.y -= speed;
		}
		if (ax > 0)
		{
			angle.y += speed;
		}

		/// X軸のカメラ回転を制限
		if (angle.x < minAngleX)
		{
			angle.x = minAngleX;
		}
		if (angle.x > maxAngleX)
		{
			angle.x = maxAngleX;
		}

		// Y軸の回転値を-3.14～3.14に収める
		if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
		if (angle.y > DirectX::XM_PI)  angle.y -= DirectX::XM_2PI;
	}

	// 突進入力処理(吹き飛んでいない時)
	if (((gamePad.GetButtonDown() & GamePad::BTN_A) || (gamePad.GetButtonHeld() & GamePad::BTN_A)) && isGround && !lunges)
	{
		// 突進ステートへ遷移
		TransitionLungesState();
	}
	else if (lungesChargeTimer > 0.0f && !lunges)
		lungesChargeTimer -= elapsedTime;

	// 弾丸入力処理
	InputProjectile();
}

// 突進ステートへ遷移
void Player::TransitionLungesState()
{
	state = State::Lunges;

	lunges = false;            // 突進する

	lungesChargeTimer = 0.0f; // 突進チャージ時間

	SoundEffectManager::Instance().PlaySoundEffect("突進チャージ");

	for (int i = 0; i < 3; i++)
	model[i]->PlayAnimation(Anim_Stop, true);
}

// 突進ステート更新
void Player::UpdateLungesState(float elapsedTime)
{
	if (moveSpeed > 0)
		moveSpeed -= 30 * elapsedTime;

	// 前方向
	DirectX::XMFLOAT3 dir;

	dir.x = transform._31;
	dir.y = transform._32;
	dir.z = transform._33;

	DirectX::XMVECTOR DIR;
	DIR = DirectX::XMLoadFloat3(&dir);
	DIR = DirectX::XMVector3Normalize(DIR);
	DirectX::XMStoreFloat3(&dir, DIR);

	//Move(dir.x, dir.z, 0.0f);

	GamePad& gamePad = Input::Instance().GetGamePad();

	float ax = gamePad.GetAxisLX();
	// カメラの回転速度
	float speed = turnSpeed * elapsedTime;
	{
		//スティックの入力値に合わせてX軸とY軸を回転
		if (ax < 0)
		{
			angle.y -= speed;
		}
		if (ax > 0)
		{
			angle.y += speed;
		}

		/// X軸のカメラ回転を制限
		if (angle.x < minAngleX)
		{
			angle.x = minAngleX;
		}
		if (angle.x > maxAngleX)
		{
			angle.x = maxAngleX;
		}
	}

	// ボタンを離したら
	if (!(gamePad.GetButtonHeld() & GamePad::BTN_A))
	{
		if(lunges < 3)
		lungesCount++;

		lungesSound = false;

		SoundEffectManager::Instance().StopSoundEffect("突進チャージ");

		if (lunges)
		{
			SoundEffectManager::Instance().StopSoundEffect("突進");
			SoundEffectManager::Instance().PlaySoundEffect("突進");

			DirectX::XMFLOAT3 pos = position;
			pos.y += height * 0.5f;

			speedHandle = sppedEffect->Play(pos, 0.5f);
		}
		

		// 移動ステートへ遷移
		TransitionMoveState();
	}
	// ボタンを押し続けているなら
	else
	{
		lungesChargeTimer += elapsedTime;

		if (lungesChargeTimer >= 0.6f)
		{
			if (!lungesSound)
			{
				SoundEffectManager::Instance().StopSoundEffect("突進チャージ");
				SoundEffectManager::Instance().PlaySoundEffect("突進ゲージマックス");

				lungesSound = true;
			}
			
			lungesChargeTimer = 0.6f; // 突進チャージ時間
			lunges      = true;       // 突進する
		}
	}
}

// 攻撃ステートへ遷移
void Player::TransitionAttackState()
{
	state = State::Attack;

	// 着地アニメーション再生
	//model->PlayAnimation(Anim_Attack, false, 0.2f);

	playerAnimeCount = 0.0f;
}

// 攻撃ステート更新処理
void Player::UpdateAttackState(float elapsedTime)
{
	for (int i = 0; i < 3; i++)
	{
		if (!model[i]->IsPlayAnimation())
		{
			// 待機ステートへ遷移
			TransitionMoveState();
		}
	}

	playerAnimeCount += elapsedTime;

	// 任意のアニメーション再生区間でのみ衝突処理判定をする
	float animationTime = static_cast<float>(playerAnimeCount) / 45.0f;
	if (animationTime >= 0.001f && animationTime <= 0.01f)
		attackCollisionFlag = true;
	else 
		attackCollisionFlag = false;

	// 衝突判定フラグが立っている場合、左手ノードとエネミーの衝突処理を行う
	if (attackCollisionFlag)
	{
		// 左手ノードとエネミーの衝突処理
		//CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
}

// ダメージステートへ遷移
void Player::TransitionDamageState()
{
	state = State::Damage;

	SoundEffectManager& sound = SoundEffectManager::Instance();
	sound.StopSoundEffect("叫び");
	sound.PlaySoundEffect("叫び", 1.2f);

	// ダメージアニメーション再生
	for(int i = 0; i < 3; i++)
	model[i]->PlayAnimation(Anim_Damage, false);
}

// ダメージステート更新処理
void Player::UpdateDamageState(float elapsedTime)
{
	if(lungesChargeTimer > 0.0f)
		lungesChargeTimer -= elapsedTime;

	if (moveSpeed > 0)
		moveSpeed -= 30 * elapsedTime;

	angle.x += DirectX::XMConvertToRadians(180) * elapsedTime;
	angle.z += DirectX::XMConvertToRadians(200) * elapsedTime;

	for (int i = 0; i < 3; i++)
	{
		// ダメージアニメーションが終わったら待機ステートへ遷移
		if (!model[i]->IsPlayAnimation() && isGround)
		{
			angle.x = DirectX::XMConvertToRadians(0);
			angle.z = DirectX::XMConvertToRadians(0);

			//! 移動ステートへ遷移
			TransitionMoveState();
		}
	}
}

// 死亡ステートへ遷移
void Player::TransitionDeathState()
{
	state = State::Death;

	// ダメージアニメーション再生
	for (int i = 0; i < 3; i++)
		model[i]->PlayAnimation(Anim_Damage, false);
}

// 死亡ステート更新処理
void Player::UpdateDeathState(float elapsedTime)
{
	angle.x += DirectX::XMConvertToRadians(180) * elapsedTime;
	angle.z += DirectX::XMConvertToRadians(200) * elapsedTime;

	velocity.y = 120.0f;
}

// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
		}

	}
}

//デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player::OnDamaged()
{
	hpShake = true;

	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// ジャンプ入力処理
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_A) //Zキー
		{
			// ジャンプ回数制限
			if (jumpCount < jumpLimit)
			{
				// ジャンプ
				Jump(jumpSpeed);
				jumpCount++;

				// ジャンプ入力した
				return true;
			}
		}
		return false;
	}
}

// 弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetCollisionPos(),
				projectile->GetRadius(),
				enemy->GetCollisionPos(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// ダメージを与える
				if (enemy->ApplyDamage(1, 0.0f))
				{
					// 吹き飛ばす
					{
						// 衝突点から敵の中心点へのベクトルを計算
						DirectX::XMVECTOR enemyPos = DirectX::XMLoadFloat3(&enemy->GetPosition());
						DirectX::XMVECTOR impactPos = DirectX::XMLoadFloat3(&position);
						DirectX::XMVECTOR impulseDir = DirectX::XMVectorSubtract(enemyPos, impactPos);

						// 正規化して方向ベクトルを得る
						impulseDir = DirectX::XMVector3Normalize(impulseDir);

						// 望むインパルスの強さを設定
						float impulseStrength = 10.0f; // この値は調整が必要です
						DirectX::XMVECTOR impulse = DirectX::XMVectorScale(impulseDir, impulseStrength);

						// インパルスをXMFLOAT3に変換
						DirectX::XMFLOAT3 impulseFloat3;
						DirectX::XMStoreFloat3(&impulseFloat3, impulse);

						enemy->AddImpulse(impulseFloat3);
					}

					// ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e, 0.9f);
					}

					{
						SoundEffectManager& sound = SoundEffectManager::Instance();
						sound.StopSoundEffect("攻撃ヒット");
						sound.PlaySoundEffect("攻撃ヒット", 0.8f);
					}

					// 弾丸破棄
					projectile->Destroy();
				}
				else
					// 弾丸破棄
					projectile->Destroy();
			}
		}
	}
}

// 当たり判定描画
void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::TreeNode("Player"))
	{
		if (ImGui::CollapsingHeader("Hp", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputInt("HP", &hp);
			ImGui::DragFloat2("HpImagePos", &hpImagePos.x);
			ImGui::DragFloat2("HpSpritePos", &hpSpritePos.x, 0.1f);
			ImGui::DragFloat("HpSpriteAdjust", &hpSpriteAdjust, 0.01f);
		}
		
		ImGui::InputInt("EXP", &exp);
		ImGui::InputInt("Level", &level);
		ImGui::InputInt("BananaNum", &bananaNum);
		
		ImGui::InputFloat("MoveSpeed", &moveSpeed);
		ImGui::Checkbox("IsGround", &isGround);
		ImGui::InputFloat3("Velocity", &velocity.x);

		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::InputFloat3("Position", &position.x);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::InputFloat3("Scale", &scale.x);
		}

		if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Lunges", &lunges);
			ImGui::InputFloat("LungesChargeTimer", &lungesChargeTimer);
		}
		ImGui::TreePop();
	}
}