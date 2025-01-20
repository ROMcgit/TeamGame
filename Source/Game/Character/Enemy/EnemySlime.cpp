#include "EneMySlime.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"

// コンストラクタ
EnemySlime::EnemySlime()
{
	model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// 幅、高さ設定
	radius = 0.5f;
	height = 1.0f;

	// 徘徊ステートへ遷移
	TransitionWanderState();
}

// デストラクタ
EnemySlime::~EnemySlime()
{
	//delete model;
}

// 更新処理
void EnemySlime::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Wander:
		UpdateWanderState(elapsedTime);
		break;
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	case State::Pursuit:
		UpdatePursuitState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	case State::WaitBattle:
		UpdateWaitBattleState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
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

	// キャラクターの状態更新処理
	UpdateCharacterState(elapsedTime);

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void EnemySlime::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void EnemySlime::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	// 縄張り範囲をデバッグ円柱描画
	//debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
	//	DirectX::XMFLOAT4(0, 1, 0, 1));

	//// ターゲット位置をデバッグ球描画
	//debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	//// 索敵範囲をデバッグ円柱描画
	//debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	//// 攻撃範囲をデバッグ円柱描画
	//debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

// 縄張り設定
void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// ターゲット位置をランダム設定
void EnemySlime::SetRandomTargetPosition()
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
void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move3D(vx, vz, moveSpeed * speedRate);
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// 徘徊ステートへ遷移
void EnemySlime::TransitionWanderState()
{
	state = State::Wander;

	// 目標地点設定
	SetRandomTargetPosition();

	// 歩きアニメーション再生
	model->PlayAnimation(Anim_WalkFWD, true);
}

// 徘徊ステート更新処理
void EnemySlime::UpdateWanderState(float elapsedTime)
{
	// 目標地点までXZ平面での距離判定
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;
	if (distSq < radius * radius)
	{
		// 次の目標地点設定
		//SetRandomTargetPosition();

		// 待機ステートへ遷移
		TransitionWaitState();
	}

	// 目標地点へ移動
	MoveToTarget(elapsedTime, 0.5f);

	// プレイヤー索敵
	if (SearchPlayer())
	{
		// 見つかったら追跡ステートへ遷移
		TransitionPursuitState();
	}
}

// 待機ステートへ遷移
void EnemySlime::TransitionWaitState()
{
	state = State::Wait;

	// タイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// 待機アニメーション再生
	model->PlayAnimation(Anim_WaitNormal, true);
}

// 待機ステート更新処理
void EnemySlime::UpdateWaitState(float elapsedTime)
{
	// タイマー処理
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		// 徘徊ステートへ遷移
		TransitionWanderState();
	}

	// プレイヤー索敵
	if (SearchPlayer())
	{
		// 見つかったら追跡ステートへ遷移
		TransitionPursuitState();
	}
}

// プレイヤー索敵
bool EnemySlime::SearchPlayer()
{
	// プレイヤーとの高低差を考慮して3Dでの距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrt(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		// 単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;
		// 前方ベクトル
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		// 2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}

	return false;
}

// 追跡ステートへ遷移
void EnemySlime::TransitionPursuitState()
{
	state = State::Pursuit;

	// 数秒間追跡するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// 歩きアニメーション再生
	model->PlayAnimation(Anim_RunFWD, true);
}

// 追跡ステート更新処理
void EnemySlime::UpdatePursuitState(float elapsedTime)
{
	// 目標地点をプレイヤー位置に設定
	targetPosition = Player::Instance().GetPosition();

	// 目標地点へ移動
	MoveToTarget(elapsedTime, 1.0f);

	// タイマー処理
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		// 待機ステートへ遷移
		TransitionWaitState();
	}

	// プレイヤーが近づくと攻撃ステートへ遷移
	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	if (dist < attackRange)
	{
		// 攻撃ステートへ遷移
		TransitionAttackState();
	}
}

// ノードとプレイヤーの衝突処理
void EnemySlime::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
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
		}
	}
}

// 攻撃ステートへ遷移
void EnemySlime::TransitionAttackState()
{
	state = State::Attack;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack1, false);
}

// 攻撃ステート更新処理
void EnemySlime::UpdateAttackState(float elapsedTime)
{
	// 任意のアニメーション再生区間でのみ衝突判定処理をする
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.1f && animationTime <= 0.35f)
	{
		// 目玉ノードとプレイヤーの衝突処理
		CollisionNodeVsPlayer("EyeBall", 0.2f);
	}

	// 攻撃アニメーションが終わったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionWaitBattleState();
	}
}

// 戦闘待機ステートへ遷移
void EnemySlime::TransitionWaitBattleState()
{
	state = State::WaitBattle;

	// 数秒間待機するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(2.0f, 3.0f);

	// 戦闘待機アニメーション再生
	model->PlayAnimation(Anim_WaitBattle, true);
}

// 戦闘待機ステート更新処理
void EnemySlime::UpdateWaitBattleState(float elapsedTime)
{
	// 目標地点をプレイヤー位置に設定
	targetPosition = Player::Instance().GetPosition();

	// タイマー処理
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		// プレイヤー攻撃範囲にいた場合は攻撃ステートへ遷移
		float vx = targetPosition.x - position.x;
		float vy = targetPosition.y - position.y;
		float vz = targetPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
		if (dist < attackRange)
		{
			// 攻撃ステートへ遷移
			TransitionAttackState();
		}
		else
		{
			// 徘徊ステートへ遷移
			TransitionWanderState();
		}

		MoveToTarget(elapsedTime, 0.0f);
	}
}

// ダメージステートへ遷移
void EnemySlime::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_GetHit, false);
}

// ダメージステート更新処理
void EnemySlime::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionWaitBattleState();
	}
}

// 死亡ステートへ遷移
void EnemySlime::TransitionDeathState()
{
	state = State::Death;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Die, false);
}

// 死亡ステート更新処理
void EnemySlime::UpdateDeathState(float elapsedTime)
{
	// ダメージアニメーションが終わったら自分を破棄
	if (!model->IsPlayAnimation())
	{
		Destroy();
	}
}

// ダメージ受けた時に呼ばれる
void EnemySlime::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡しと時に呼ばれる
void EnemySlime::OnDead()
{
	//Destroy();

	// 死亡ステートへ遷移
	TransitionDeathState();
}
