#include <imgui.h>
#include "Game/Character/Player.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileUnko.h"

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

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	hp = hpDamage = maxHp = 100;

	// 当たり判定
	radius = 0.9f;
	height = 2.4f;
	collisionOffset = { 0, -1.0f, 0 };

	// ヒットエフェクト読み込み
	hitEffect = std::make_unique <Effect>("Data/Effect/Hit.efk");

	// 移動ステートへ遷移
	TransitionMoveState();

	//! HPゲージの位置
	hpSpritePos = { 100.0f, 640.0f };
	hpSpriteShakePosY = hpSpritePos.y;

	for (int i = 0; i < 3; i++)
	{
		hpSprite[i + 1] = std::make_unique<Sprite>();
	}
}

// デストラクタ
Player::~Player()
{
	//delete hitEffect;

	//delete model;
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

	// ムービー中なら待機ステートへ遷移
	if (movieScene)
	{
		// 全ての弾を破棄する
		int projectileCount = projectileManager.GetProjectileCount();
		for (int i = 0; i < projectileCount; ++i)
		{
			Projectile* projectile = projectileManager.GetProjectile(i);

			// 弾破棄
			projectile->Destroy();
		}

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

	// プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	// HP管理
	HpControll(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// 弾丸と敵に衝突処理
	CollisionProjectilesVsEnemies();

#if 0
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 移動処理
	float moveSpeed = this->moveSpeed * elapsedTime;
	position.x += moveVec.x * moveSpeed;
	position.z += moveVec.z * moveSpeed;

	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();
#endif

#if 0
	// 移動操作
	float moveSpeed = 5.0f * elapsedTime;
	{
		// X移動
		if (ax == 1)
		{
			position.x += 0.1;
		}
		else if (ax == -1)
		{
			position.x -= 0.1;
		}

		// Z移動
		if (ay == 1)
		{
			position.z += 0.1;
		}
		else if (ay == -1)
		{
			position.z -= 0.1;
		}
	}

	// 回転処理
	float rotateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;
	if (gamePad.GetButton() & GamePad::BTN_A)
	{
		// X軸回転操作
		angle.x += 0.1;
	}

	if (gamePad.GetButton() & GamePad::BTN_B)
	{
		// Y軸回転操作
		angle.y += 0.1;
	}

	if (gamePad.GetButton() & GamePad::BTN_X)
	{
		// Z軸回転操作
		angle.z += 0.1;
	}
#endif

	// 当たり判定の位置設定
	CollisionPosSettings();

	// オブジェクト行列を更新
	UpdateTransform();

	// モデルアニメーション更新処理
	for (int i = 0; i < 3; i++)
	{
		model[i]->UpdateAnimation(elapsedTime);

		// モデル更新処理
		model[i]->UpdateTransform(transform);
	}
}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (int i = 0; i < 3; i++)
	shader->Draw(dc, model[i].get());

	// 弾丸描画処理
	projectileManager.Render(dc, shader);
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
			137, 25,
			0, 0, textureWidth, textureHeight,
			0,
			0.0f, 0.0f, 0.0f, 1);

		if (hp > 25)
		{
			// ダメージ(赤色)
			hpSprite[2]->Render(dc,
				hpSpritePos.x, hpSpritePos.y,
				hpDamage * 1.34f, 25,
				0, 0, textureWidth, textureHeight,
				0,
				1, 0, 0, 1);

			// ダメージ(緑色)
			hpSprite[3]->Render(dc,
				hpSpritePos.x, hpSpritePos.y,
				hp * 1.34f, 25,
				0, 0, textureWidth, textureHeight,
				0,
				0, 1, 0, 1);
		}
		else
		{
			textureWidth = static_cast<float>(hpSprite[1]->GetTextureWidth());
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

	// 直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X) //Cキー
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

		// ターゲット(デフォルトではプレイヤーの前方)
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 10.0f; // 10.0f は任意の距離
		target.y = pos.y + dir.y * 10.0f;
		target.z = pos.z + dir.z * 10.0f;

		// 一番近くの敵をターゲット
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			// 敵との距離判定
			std::unique_ptr<Enemy>& enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5f;
			}
		}

		//発射
		ProjectileUnko * projectile = new ProjectileUnko(&projectileManager);
		projectile->Launch(dir, pos);
		/*ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);*/
		
		//projectileManager.Register(projectile);
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
	if (lunges && lungesTimer > 0.0f)
	{
		Move(dir.x, dir.z, 2000.0f);

		invincibleTimer = 0.5f;
		lungesTimer -= elapsedTime;
	}
	else
	{
		lunges = false;

		// 移動処理
		Move(dir.x, dir.z, 15);
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	float ax = gamePad.GetAxisLX();
	// カメラの回転速度
	float speed = turnSpeed * elapsedTime;
	{
		//スティックの入力値に合わせてX軸とY軸を回転
		if (ax == -1)
		{
			angle.y -= speed;
		}
		if (ax == 1)
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
	if (gamePad.GetButtonDown() & GamePad::BTN_A && isGround)
	{
		// 攻撃ステートへ遷移
		TransitionLungesState();
	}

	// 弾丸入力処理
	InputProjectile();
}

// 突進ステートへ遷移
void Player::TransitionLungesState()
{
	state = State::Lunges;

	lunges = false;            // 突進する

	lungesChargeTimer = 0.0f; // 突進チャージ時間

	lungesTimer = 0.0f;       // 突進時間

	for (int i = 0; i < 3; i++)
	model[i]->PlayAnimation(Anim_Stop, true);
}

// 突進ステート更新
void Player::UpdateLungesState(float elapsedTime)
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

	Move(dir.x, dir.z, 0.0f);

	GamePad& gamePad = Input::Instance().GetGamePad();

	float ax = gamePad.GetAxisLX();
	// カメラの回転速度
	float speed = turnSpeed * elapsedTime;
	{
		//スティックの入力値に合わせてX軸とY軸を回転
		if (ax == -1)
		{
			angle.y -= speed;
		}
		if (ax == 1)
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
		// 移動ステートへ遷移
		TransitionMoveState();
	}
	// ボタンを押し続けているなら
	else
	{
		lungesChargeTimer += elapsedTime;

		if (lungesChargeTimer >= 0.5f)
		{
			lungesChargeTimer = 0.5f; // 突進チャージ時間
			lunges      = true;       // 突進する
			lungesTimer = 0.5f;       // 突進チャージ時間
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

	// ダメージアニメーション再生
	for(int i = 0; i < 3; i++)
	model[i]->PlayAnimation(Anim_Damage, false);
}

// ダメージステート更新処理
void Player::UpdateDamageState(float elapsedTime)
{
	angle.x += DirectX::XMConvertToRadians(180) * elapsedTime;
	angle.z += DirectX::XMConvertToRadians(200) * elapsedTime;

	for (int i = 0; i < 3; i++)
	{
		// ダメージアニメーションが終わったら待機ステートへ遷移
		if (!model[i]->IsPlayAnimation() && isGround)
		{
			angle.x = DirectX::XMConvertToRadians(0);
			angle.z = DirectX::XMConvertToRadians(0);

			TransitionMoveState();
		}
	}
}

// 死亡ステートへ遷移
void Player::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	//model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player::UpdateDeathState(float elapsedTimae)
{
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
				if (enemy->ApplyDamage(1, 1.0f))
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
						hitEffect->Play(e);
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
		ImGui::InputInt("HP", &hp);
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
			ImGui::InputFloat("LungesTimer", &lungesTimer);
		}
		ImGui::TreePop();
	}
}