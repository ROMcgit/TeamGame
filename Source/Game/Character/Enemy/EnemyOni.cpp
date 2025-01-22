#include "EnemyOni.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"

// コンストラクタ
EnemyOni::EnemyOni()
{
	model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// 幅、高さ設定
	radius = 0.5f;
	height = 1.0f;
}

// デストラクタ
EnemyOni::~EnemyOni()
{
	//delete model;
}

// 更新処理
void EnemyOni::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	case State::Move:
		UpdateMoveState(elapsedTime);
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
void EnemyOni::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void EnemyOni::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void EnemyOni::DrawDebugPrimitive()
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

// ノードとプレイヤーの衝突処理
void EnemyOni::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
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
		Player0_Onigokko& player = Player0_Onigokko::Instance();
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

// 待機ステートへ遷移
void EnemyOni::TransitionWaitState()
{
	state = State::Wait;

	stateChangeWaitTimer = 0.5f;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void EnemyOni::UpdateWaitState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;
}

// 移動ステートへ遷移
void EnemyOni::TransitionMoveState()
{
	state = State::Move;
}

// 移動ステート更新処理
void EnemyOni::UpdateMoveState(float elapsedTime)
{
}

// 威嚇ステートへ遷移
void EnemyOni::TransitionThreatState()
{
	state = State::Threat;

	stateChangeWaitTimer = 0.5f;
}

// 威嚇ステート更新処理
void EnemyOni::UpdateThreatState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;
}

// 追跡ステートへ遷移
void EnemyOni::TransitionTrackingState()
{
	state = State::Tracking;
}

// 追跡ステート更新処理
void EnemyOni::UpdateTrackingState(float elapsedTime)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();

	//! プレイヤーに向かって移動する
	MoveToTarget(elapsedTime, 100);
}

// 攻撃ステートへ遷移
void EnemyOni::TransitionAttackState()
{
	state = State::Attack;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack, false);
}

// 攻撃ステート更新処理
void EnemyOni::UpdateAttackState(float elapsedTime)
{
	
}

// ダメージステートへ遷移
void EnemyOni::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Attack, false);
}

// ダメージステート更新処理
void EnemyOni::UpdateDamageState(float elapsedTime)
{
}

// 死亡ステートへ遷移
void EnemyOni::TransitionDeathState()
{
	state = State::Death;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void EnemyOni::UpdateDeathState(float elapsedTime)
{
	// ダメージアニメーションが終わったら自分を破棄
	if (!model->IsPlayAnimation())
	{
		Destroy();
	}
}

// ダメージ受けた時に呼ばれる
void EnemyOni::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡しと時に呼ばれる
void EnemyOni::OnDead()
{
	//Destroy();

	// 死亡ステートへ遷移
	TransitionDeathState();
}
