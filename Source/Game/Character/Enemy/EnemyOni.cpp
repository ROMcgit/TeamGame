#include "EnemyOni.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"
#include "Audio/BgmManager.h"

#include <algorithm>
#include "EnemyDarumasangaKoronda.h"

bool EnemyOni::tracking = false;

// コンストラクタ
EnemyOni::EnemyOni(bool perception)
{
	//! 察知しているか
	this->perception = perception;

	model = std::make_unique<Model>("Data/Model/0.Onigokko/Oni/Oni.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.6f;
	height = 5.0f;

	opacity = 0;
	SetOpacityChange(1.0f, 0.8f);

	TransitionEntryState();
}

// デストラクタ
EnemyOni::~EnemyOni()
{
	//delete model;
}

// 更新処理
void EnemyOni::Update(float elapsedTime)
{
	if (Timer::GetTimeM_Int() <= 0 && Timer::GetTimeS_Int() <= 0 && !deathState)
	{
		TransitionDeathState();
		deathState = true;
	}

	// ステート毎の更新処理
	switch (state)
	{
	// 登場
	case State::Entry:
		UpdateEntryState(elapsedTime);
		break;
	// 待機
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	// 移動
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	// 笑う
	case State::Laugh:
		UpdateLaughState(elapsedTime);
		break;
	// 追跡
	case State::Tracking:
		UpdateTrackingState(elapsedTime);
		break;
	// 疲れる
	case State::Tired:
		UpdateTiredState(elapsedTime);
		break;
	// 攻撃
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	// 死亡
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// キャラクターの状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// プレイヤーとの当たり判定
	CollisionVsPlayer();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void EnemyOni::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if ((dist < 7000 || G0_Onigokko::movieScene) && opacity > 0)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// HPなどの描画
void EnemyOni::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void EnemyOni::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	//Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	debugRender->DrawCylinder(position, searchRange, 2.0f, { 1,1,1,1 });

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

void EnemyOni::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyOni"))
	{
		ImGui::InputFloat("Dist", &dist);
		ImGui::InputFloat3("Velocity", &velocity.x);

		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::DragFloat3("Position", &position.x, 0.01f);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::DragFloat3("Angle", &a.x, 0.01f);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		}
		if (ImGui::CollapsingHeader("Collision"))
		{
			ImGui::DragFloat("Radius", &radius, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
		}

		ImGui::TreePop();
	}
}

// 登場ステートへ遷移
void EnemyOni::TransitionEntryState()
{
	state = State::Entry;

	stateChangeWaitTimer = 1.0f;

	model->PlayAnimation(Anim_Entry, false);
}

// 登場ステート更新処理
void EnemyOni::UpdateEntryState(float elapsedTime)
{
	if(!model->IsPlayAnimation())
		stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
		TransitionWaitState();
}

// 待機ステートへ遷移
void EnemyOni::TransitionWaitState()
{
	state = State::Wait;

	stateChangeWaitTimer = 1.0f;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void EnemyOni::UpdateWaitState(float elapsedTime)
{
	//! ムービーシーンでないなら、待ち時間を減らす
	if (!G0_Onigokko::movieScene)
		stateChangeWaitTimer -= elapsedTime;

	Player0_Onigokko& player = Player0_Onigokko::Instance();

	targetPosition = player.GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if ((dist < searchRange * searchRange && player.GetInvincibleTimer() <= 0) || tracking)
		//! 威嚇ステートへ遷移
		TransitionLaughState();
	else if (stateChangeWaitTimer <= 0.0f)
		TransitionMoveState();
}

// 移動ステートへ遷移
void EnemyOni::TransitionMoveState()
{
	state = State::Move;

	stateChangeWaitTimer = 10.0f;

	setMoveTarget = false;

	model->PlayAnimation(Anim_Move, true);
}

// 移動ステート更新処理
void EnemyOni::UpdateMoveState(float elapsedTime)
{
	if(!setMoveTarget)
	{
		if (!perception)
		{
			moveTarget.x = position.x + (rand() % 200 + 200 * (rand() % 2 == 1 ? -1 : 1));
			moveTarget.z = position.z + (rand() % 200 + 200 * (rand() % 2 == 1 ? -1 : 1));

			float setPosX = 295.0f;
			moveTarget.x = std::clamp(moveTarget.x, -setPosX, setPosX);
			float setPosZ = 287.0f;
			moveTarget.z = std::clamp(moveTarget.z, -setPosZ, setPosZ);
		}
		else
		{
			Player0_Onigokko& player = Player0_Onigokko::Instance();
			moveTarget.x = player.GetPosition().x + (3 * (rand() % 2 == 1 ? -1 : 1));
			moveTarget.z = player.GetPosition().z + (3 * (rand() % 2 == 1 ? -1 : 1));
		}

		setMoveTarget = true;
	}

	// 移動位置に移動
	float moveSpeed = perception ? 10 : 60;

	MoveTarget(elapsedTime, moveSpeed);

	float vxm = moveTarget.x - position.x;
	float vzm = moveTarget.z - position.z;
	float d = vxm * vxm + vzm * vzm;
	if (d < (radius * radius) * 2)
	{
		MoveTarget(elapsedTime, 0);
		// 待機ステートへ遷移
		TransitionWaitState();
	}

	Player0_Onigokko& player = Player0_Onigokko::Instance();
	targetPosition = player.GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if ((dist < searchRange * searchRange && player.GetInvincibleTimer() <= 0) || tracking)
		//! 威嚇ステートへ遷移
		TransitionLaughState();
	else if (stateChangeWaitTimer <= 0.0f)
		//! 待機ステートへ遷移
		TransitionWaitState();
}

// 威嚇ステートへ遷移
void EnemyOni::TransitionLaughState()
{
	if (!tracking) tracking = true;

	BgmManager::Instance().ChangeBgmStatus("おにごっこ", 1.0f, 3.0f);

	state = State::Laugh;

	stateChangeWaitTimer = 0.5f;

	velocity.x = velocity.z = 0;

	//! コントラスト
	SetContrastChange(1.5f, 0.5f);

	//! サチュレーション
	SetSaturationChange(1.0f, 0.5f);

	//! カラーフィルター
	SetColorFilterChange(DirectX::XMFLOAT3(3.0f, 1.3f, 1.35f), 0.5f);

	//! クロマティックアベレーション
	SetChromaticAberrationChange(0.03f, 1.5f);

	model->PlayAnimation(Anim_Laugh, false);
}

// 威嚇ステート更新処理
void EnemyOni::UpdateLaughState(float elapsedTime)
{
	if(!model->IsPlayAnimation())
		stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
		TransitionTrackingState();
}

// 追跡ステートへ遷移
void EnemyOni::TransitionTrackingState()
{
	state = State::Tracking;

	stateChangeWaitTimer = 5.0f;

	model->PlayAnimation(Anim_Tracking, true);
}

// 追跡ステート更新処理
void EnemyOni::UpdateTrackingState(float elapsedTime)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if (dist < 1500)
		//! プレイヤーに向かって移動する
		MoveToTarget(elapsedTime, 5);
	else
		//! プレイヤーの位置制に向かって移動する
		//! プレイヤーに向かって移動する
		MoveToTarget(elapsedTime, 80);

	stateChangeWaitTimer -= elapsedTime;

	// 一定時間で疲れる
	if (stateChangeWaitTimer <= 0.0f)
		TransitionTiredState();
}

// 疲れるステート遷移
void EnemyOni::TransitionTiredState()
{
	state = State::Tired;

	BgmManager::Instance().ChangeBgmStatus("おにごっこ", 1.0f, 1.0f);

	tracking = false;

	//! ポストエフェクトを元に戻す
	SetPostEffectStatusResetChange();

	model->PlayAnimation(Anim_Tired, false);
}

// 疲れるステート更新処理
void EnemyOni::UpdateTiredState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
		//! 待機ステートへ遷移
		TransitionWaitState();
}

// 攻撃ステートへ遷移
void EnemyOni::TransitionAttackState()
{
	state = State::Attack;

	G0_Onigokko::movieScene = true;

	stateChangeWaitTimer = 2.2f;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack, false);
}

// 攻撃ステート更新処理
void EnemyOni::UpdateAttackState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		position = { 50, 5, 80 };

		//! ポストエフェクトを元に戻す
		SetPostEffectStatusResetChange();

		TransitionWaitState();
	}
}

// 死亡ステートへ遷移
void EnemyOni::TransitionDeathState()
{
	state = State::Death;

	angle.y = DirectX::XMConvertToRadians(180);

	stateChangeWaitTimer = 2.0f;
}

// 死亡ステート更新処理
void EnemyOni::UpdateDeathState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (!playAnimation && stateChangeWaitTimer <= 0.0f)
	{
		// ダメージアニメーション再生
		model->PlayAnimation(Anim_Death, false);

		playAnimation = true;
	}

	if (!model->IsPlayAnimation() && playAnimation)
		SetOpacityChange(0.0f, 1.5f);
}

// ダメージ受けた時に呼ばれる
void EnemyOni::OnDamaged()
{
}

// 死亡しと時に呼ばれる
void EnemyOni::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// プレイヤーとの当たり判定
void EnemyOni::CollisionVsPlayer()
{
	Player0_Onigokko& player = Player0_Onigokko::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		position,
		radius,
		height,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition
	) && player.GetInvincibleTimer() <= 0)
	{
		TransitionAttackState();
		player.ApplyDamage(1, 10, 0);
	}
}

// 移動位置に移動
void EnemyOni::MoveTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = moveTarget.x - position.x;
	float vz = moveTarget.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move3D(vx, vz, moveSpeed * speedRate);
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}
