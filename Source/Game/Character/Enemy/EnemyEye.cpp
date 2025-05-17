#include "EnemyEye.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"
#include "EnemyOni.h"

#include <algorithm>

// コンストラクタ
EnemyEye::EnemyEye()
{
	model = std::make_unique<Model>("Data/Model/0.Onigokko/Eye/Eye.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.02f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 2.11f;
	height = 1.95f;

	rotationR = rand() % 2 == 1 ? true : false;

	TransitionEntryState();

	verticalMove = rand() % 2 == 1 ? true : false;
}

// デストラクタ
EnemyEye::~EnemyEye()
{
	//delete model;
}

// 更新処理
void EnemyEye::Update(float elapsedTime)
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
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// キャラクターの状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// プレイヤーとの衝突処理
	CollisionVsPlayer();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void EnemyEye::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if (dist < 2000 && !G0_Onigokko::movieScene && !EnemyOni::tracking)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// HPなどの描画
void EnemyEye::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void EnemyEye::DrawDebugPrimitive()
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

void EnemyEye::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyEye"))
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

// 待機ステートへ遷移
void EnemyEye::TransitionWaitState()
{
	state = State::Wait;

	stateChangeWaitTimer = 1.0f;
}

// 待機ステート更新処理
void EnemyEye::UpdateWaitState(float elapsedTime)
{
	//! 横移動
	if (!verticalMove) velocity.x += moveMinus ? -1 : 1;
	else velocity.z += moveMinus ? -1 : 1;

	velocity.x = std::clamp(velocity.x, -velocityXMax, velocityXMax);
	velocity.z = std::clamp(velocity.z, -velocityZMax, velocityZMax);

	actionTimer += elapsedTime;
	if (actionTimer > 5.0f)
	{
		velocityXMax = rand() % 6;
		velocityZMax = rand() % 6;

		moveMinus = !moveMinus;
		actionTimer = 0.0f;
	}


	//! ムービーシーンでないなら、待ち時間を減らす
	if (!G0_Onigokko::movieScene)
		stateChangeWaitTimer -= elapsedTime;

	Player0_Onigokko& player = Player0_Onigokko::Instance();

	targetPosition = player.GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;

	angle.y += DirectX::XMConvertToRadians(100) * (rotationR ? 1 : -1) * elapsedTime;

	if (SearchPlayer() && player.GetInvincibleTimer() <= 0)
		EnemyOni::tracking = true;
}
// 死亡ステートへ遷移
void EnemyEye::TransitionDeathState()
{
	angle.y = DirectX::XMConvertToRadians(180);

	stateChangeWaitTimer = 2.0f;
}

// 死亡ステート更新処理
void EnemyEye::UpdateDeathState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (!playAnimation && stateChangeWaitTimer <= 0.0f)
	{

		playAnimation = true;
	}

	if (!model->IsPlayAnimation() && playAnimation)
		SetOpacityChange(0.0f, 1.5f);
}

// ダメージ受けた時に呼ばれる
void EnemyEye::OnDamaged()
{
}

// 死亡しと時に呼ばれる
void EnemyEye::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// プレイヤーとの衝突処理
void EnemyEye::CollisionVsPlayer()
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
		outPosition) && !EnemyOni::tracking)
	{
		player.SetPosition(outPosition);
	}
}

// プレイヤーを探す
bool EnemyEye::SearchPlayer()
{
	// プレイヤーとの高低差を考慮して3Dで距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player0_Onigokko::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		// 単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;

		// 方向ベクトル化
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		// 2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		float threshold = cosf(DirectX::XMConvertToRadians(30.0f)); // 30度をラジアンに変換
		if (dot > threshold)
		{
			return true;
		}
	}
	return false;
}

// 移動位置に移動
void EnemyEye::MoveTarget(float elapsedTime, float speedRate)
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

// 登場ステートへ遷移
void EnemyEye::TransitionEntryState()
{
	state = State::Entry;
}

// 登場ステート更新処理
void EnemyEye::UpdateEntryState(float elapsedTime)
{
	if (Player0_Onigokko::Instance().GetMovieTime() <= 0.0f)
		TransitionWaitState();
}
