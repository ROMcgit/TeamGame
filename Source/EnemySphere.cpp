#include "EnemySphere.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"
#include <ProjectileStraight.h>
#include <ProjectilePlayer.h>
#include <Input/GamePad.h>
#include <Input/Input.h>
#include <EnemyManager.h>
#include "WallManager.h"
#include "SceneTitle.h"
#include "EnemyStrong.h"

// コンストラクタ
EnemySphere::EnemySphere()
{
	model = std::make_unique<Model>("Data/Model/Enemy/enemy01.mdl");

	// ヒットエフェクト読み込み
	hitEffect = std::make_unique<Effect>("Data/Effect/Blast.efk");
	bariaEffect = std::make_unique<Effect>("Data/Effect/Baria.efk");

	// Audio クラスのインスタンス化と初期化
	Audio& audioManager = Audio::Instance();

	sound[0] = audioManager.LoadAudioSource("Data/Audio/bat.wav");
	sound[1] = audioManager.LoadAudioSource("Data/Audio/crash.wav");
	sound[2] = audioManager.LoadAudioSource("Data/Audio/金属の衝突.wav");
	sound[3] = audioManager.LoadAudioSource("Data/Audio/中パンチ.wav");

	attackSound = audioManager.LoadAudioSource("Data/Audio/弾発射.wav");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0;

	// 幅、高さ設定
	radius = 0.3f;
	height = 0.5f;

	// 徘徊ステートへ遷移
	TransitionIdleState();

	health = 1;
}

// デストラクタ
EnemySphere::~EnemySphere()
{
	//delete model;
}

// 更新処理
void EnemySphere::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	//case State::Damage:
	//	UpdateDamageState(elapsedTime);
	//	break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);

	// 弾とプレイヤーの当たり判定
	CollisionProjectilesVsPlayer();

	// 弾と敵の当たり判定
	CollisionProjectilesVsEnemy();

	// 弾と壁の当たり判定
	CollisionProjectilesVsWall();

	// 位置調整
	PositionControll();

	// HP制御
	HpControll();

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	if(damageWaitTime > 0)
	damageWaitTime -= 1;

	if (attackWait > 0)
		attackWait -= 1;

	if (scale.x <= 0.001f && scale.y <= 0.001f && scale.z <= 0.001f)
	{
		scale.x += 0.0001f;
		scale.y += 0.0001f;
		scale.z += 0.0001f;
	}

	//! スコアのインスタンス
	SceneTitle& title = SceneTitle::Instance();
	if (title.score > 100000) this->ApplyDamage(100, 0);
}

// 描画処理
void EnemySphere::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
	// 弾丸描画処理
	projectileManager.Render(dc, shader);
}

// デバッグプリミティブ描画
void EnemySphere::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	//Enemy::DrawDebugPrimitive();

	//DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	//// 縄張り範囲をデバッグ円柱描画
	//debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
	//	DirectX::XMFLOAT4(0, 1, 0, 1));

	// ターゲット位置をデバッグ球描画
	//debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	// 弾丸デバッグプリミティブ描画
	//projectileManager.DrawDebugPrimitive();

	//// 索敵範囲をデバッグ円柱描画
	//debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	//// 攻撃範囲をデバッグ円柱描画
	//debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

// 縄張り設定
void EnemySphere::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// 位置調整
void EnemySphere::PositionControll()
{
	position.z = 0;
	if (position.x < -6.66) position.x = -6.66;
	if (position.x > 6.6) position.x = 6.6;
}

// 弾丸とプレイヤーの衝突処理
void EnemySphere::CollisionProjectilesVsPlayer()
{
	Player& player = Player::Instance();

	GamePad& gamePad = Input::Instance().GetGamePad();

	// 全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		if (gamePad.GetButtonDown() & GamePad::BTN_START || gamePad.GetButtonDown() & GamePad::BTN_SPACE && damageWaitTime <= 0) 
			damageWaitTime = 60;
		
			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				player.GetPosition(),
				player.GetRadius(),
				player.GetHeight(),
				outPosition)
				&& player.health > 0)
			{
				if (damageWaitTime > 0)
				{
					// 弾丸破棄
					projectile->Destroy();

					sound[0]->Play(false, 0.4f);

					const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();

					// 前方向
					DirectX::XMFLOAT3 dir;

					dir.x = position.x - playerPosition.x;
					dir.y = position.y - playerPosition.y;
					dir.z = position.z - playerPosition.z;

					DirectX::XMVECTOR DIR;
					DIR = DirectX::XMLoadFloat3(&dir);
					DIR = DirectX::XMVector3Normalize(DIR);
					DirectX::XMStoreFloat3(&dir, DIR);

					// 発射位置(プレイヤーの腰あたり)
					DirectX::XMFLOAT3 pos;
					pos.x = playerPosition.x;
					pos.y = playerPosition.y + player.GetHeight() + 0.2;
					pos.z = playerPosition.z;

					ProjectilePlayer* projectile = new ProjectilePlayer(&projectileManager);
					projectile->Launch(dir, pos);

					Player& player = Player::Instance();

					DirectX::XMFLOAT3 e = playerPosition;
					e.y += player.GetHeight();
					hitEffect->Play(e, 0.01f);
				}

				// ダメージを与える
				else if(damageWaitTime <= 0)
				{
					if (player.ApplyDamage(10, 3.0f))
					{
						// 弾丸破棄
						projectile->Destroy();

						const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();

						// 前方向
						DirectX::XMFLOAT3 dir;

						dir.x = position.x - playerPosition.x;
						dir.y = position.y - playerPosition.y;
						dir.z = position.z - playerPosition.z;

						DirectX::XMVECTOR DIR;
						DIR = DirectX::XMLoadFloat3(&dir);
						DIR = DirectX::XMVector3Normalize(DIR);
						DirectX::XMStoreFloat3(&dir, DIR);

						// 発射位置(プレイヤーの腰あたり)
						DirectX::XMFLOAT3 pos;
						pos.x = playerPosition.x;
						pos.y = playerPosition.y + player.GetHeight();
						pos.z = playerPosition.z;

						ProjectilePlayer* projectile = new ProjectilePlayer(&projectileManager);
						projectile->Launch(dir, pos);

						Player& player = Player::Instance();

						DirectX::XMFLOAT3 e = playerPosition;
						e.y += player.GetHeight();
						hitEffect->Play(e, 0.01f);
					}
				}
			}
	}
}

// 弾と敵の当たり判定
void EnemySphere::CollisionProjectilesVsEnemy()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);
		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);
			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				projectile->GetPosition(),
				projectile->GetRadius(),
				position,
				radius,
				outPosition))
			{
				if (attackWait <= 0)
				{
					sound[3]->Play(false, 1.0f);
					this->ApplyDamage(1, 2);
					projectile->Destroy();
				}
			}
		}
	}
}

// 弾と壁の当たり判定
void EnemySphere::CollisionProjectilesVsWall()
{
	WallManager& wallManager = WallManager::Instance();

	int projectileCount = projectileManager.GetProjectileCount();
	int wallCount = wallManager.GetWallCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);
		for (int j = 0; j < wallCount; ++j)
		{
			Wall* wall = wallManager.GetWall(j);
			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				wall->GetPosition(),
				wall->GetRadius(),
				wall->GetHeight(),
				outPosition))
			{
				if (wall->ApplyDamage(1, 1))
				{
					// 弾丸破棄
					projectile->Destroy();

					sound[2]->Play(false, 1.0f);

					// 前方向
					DirectX::XMFLOAT3 dir;

					dir.x = position.x - wall->GetPosition().x;
					dir.y = position.y - wall->GetPosition().y;
					dir.z = position.z - wall->GetPosition().z;

					DirectX::XMVECTOR DIR;
					DIR = DirectX::XMLoadFloat3(&dir);
					DIR = DirectX::XMVector3Normalize(DIR);
					DirectX::XMStoreFloat3(&dir, DIR);

					// 発射位置(プレイヤーの腰あたり)
					DirectX::XMFLOAT3 pos;
					pos.x = wall->GetPosition().x;
					pos.y = wall->GetPosition().y + wall->GetHeight();
					pos.z = wall->GetPosition().z;

					ProjectilePlayer* projectile = new ProjectilePlayer(&projectileManager);
					projectile->Launch(dir, pos);

					Player& player = Player::Instance();

					DirectX::XMFLOAT3 e = wall->GetPosition();
					wall->GetPosition().y + wall->GetHeight() + 0.4f;
					hitEffect->Play(e, 0.02f);
				}
			}
		}
	}
}

// ターゲット位置をランダム設定
void EnemySphere::SetRandomTargetPosition()
{
	// 縄張り範囲内でランダムな位置を生成
	float randomX = Mathf::RandomRange(territoryOrigin.x - territoryRange, territoryOrigin.x + territoryRange);
	float randomZ = Mathf::RandomRange(territoryOrigin.z - territoryRange, territoryOrigin.z + territoryRange);

	// Y座標はスライムの高さに設定するか、固定の地面の高さに設定する
	float randomY = territoryOrigin.y; // または固定値、例: 0.0f

	// ターゲット位置に設定
	targetPosition.x = randomX;
	targetPosition.y = randomY;
	targetPosition.z = randomZ;
}

// 目標地点へ移動
void EnemySphere::MoveToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// 徘徊ステートへ遷移
void EnemySphere::TransitionIdleState()
{
	state = State::Idle;

	// 目標地点設定
	//SetRandomTargetPosition();

	positionRandamuL = 240;
	positionRandamuR = 240;

	// 歩きアニメーション再生
	model->PlayAnimation(Anim_Idle, true);
}

// TODO:行動処理(敵)
void EnemySphere::UpdateIdleState(float elapsedTime)
{
	TransitionIdleState();

	if (moveR == false)
	{
		velocity.x = -1;
		waitL++;
	}
	if (waitL > positionRandamuL)
	{
		positionRandamuL = 0;
		moveR = true;
		waitL = 0;
	}

	if (moveR == true)
	{
		velocity.x = 1;
		waitR++;
	}
	if (waitR > positionRandamuR)
	{
		positionRandamuR = 0;
		moveR = false;
		waitR = 0;
	}

	// 弾の発射(敵)
	if (waitCount > 300)
	{
		waitCount = 0;
		TransitionAttackState();
	}
	else
	{
		waitCount++;
	}
	
}

//// プレイヤー索敵
//bool EnemySphere::SearchPlayer()
//{
//	// プレイヤーとの高低差を考慮して3Dでの距離判定をする
//	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
//	float vx = playerPosition.x - position.x;
//	float vy = playerPosition.y - position.y;
//	float vz = playerPosition.z - position.z;
//	float dist = sqrt(vx * vx + vy * vy + vz * vz);
//
//	if (dist < searchRange)
//	{
//		float distXZ = sqrtf(vx * vx + vz * vz);
//		// 単位ベクトル化
//		vx /= distXZ;
//		vz /= distXZ;
//		// 前方ベクトル
//		float frontX = sinf(angle.y);
//		float frontZ = cosf(angle.y);
//		// 2つのベクトルの内積値で前後判定
//		float dot = (frontX * vx) + (frontZ * vz);
//		if (dot > 0.0f)
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

// ノードとプレイヤーの衝突処理
void EnemySphere::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	// ノードの位置と当たり判定を行う
	Model::Node* node = model->FindNode(nodeName);
	if (node != nullptr)
	{
		// ノードのワールド座標
		DirectX::XMFLOAT3 nodePosition(
			node->worldTransform._41,
			node->worldTransform._42,
			node->worldTransform._43
		);

		// 当たり判定表示
		Graphics::Instance().GetDebugRenderer()->DrawSphere(
			nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
		);

		// プレイヤーと当たり判定
		Player& player = Player::Instance();
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition))
		{
			// ダメージを与える
			if (player.ApplyDamage(1, 0.5f))
			{
				// 敵を吹っ飛ばすベクトルを算出
				DirectX::XMFLOAT3 vec;
				vec.x = outPosition.x - nodePosition.x;
				vec.z = outPosition.z - nodePosition.z;
				float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
				vec.x /= length;
				vec.z /= length;

				/// XZ平面に吹っ飛ばす力をかける
				float power = 10.0f;
				vec.x *= power;
				vec.z *= power;
				// Y方向にも力をかける
				vec.y = 5.0f;

				// 吹っ飛ばす
				player.AddImpulse(vec);
			}
		}
	}
}

// 攻撃ステートへ遷移
void EnemySphere::TransitionAttackState()
{
	state = State::Attack;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack, false);
}

void EnemySphere::UpdateAttackState(float elapsedTime)
{
	if (waitCount == 0)
	{
		attackWait = 30;
		const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();

		// 前方向
		DirectX::XMFLOAT3 dir;

		dir.x = playerPosition.x - position.x;
		dir.y = playerPosition.y - position.y;
		dir.z = playerPosition.z - position.z;

		DirectX::XMVECTOR DIR;
		DIR = DirectX::XMLoadFloat3(&dir);
		DIR = DirectX::XMVector3Normalize(DIR);
		DirectX::XMStoreFloat3(&dir, DIR);

		// 発射位置(プレイヤーの腰あたり)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y - 0.2;
		pos.z = position.z;

		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);

		DirectX::XMFLOAT3 e = position;
		e.y += 0.4f;

		bariaEffect->Play(e, 0.22f);

		attackSound->Play(false, 0.6f);
	}

	if(waitCount > 60)
	TransitionIdleState();

	waitCount++;
}

// ダメージステートへ遷移
void EnemySphere::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Damage, false);
}

// 死亡ステートへ遷移
void EnemySphere::TransitionDeathState()
{
	state = State::Death;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void EnemySphere::UpdateDeathState(float elapsedTime)
{
	// ダメージアニメーションが終わったら自分を破棄
	if (!model->IsPlayAnimation())
	{
		Player& player = Player::Instance();
		SceneTitle& title = SceneTitle::Instance();
		title.score += 100;

		sound[1]->Play(false, 1.0f);

		position.y + 0.5f;
		hitEffect->Play(position, 0.02f);
		Destroy();
	}
}

// ダメージ受けた時に呼ばれる
void EnemySphere::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡しと時に呼ばれる
void EnemySphere::OnDead()
{
	//Destroy();

	// 死亡ステートへ遷移
	TransitionDeathState();
}
