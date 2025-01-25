#include "GS0_OniGokko.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_OniGokko.h"
#include "Other/Collision.h"
#include "Game/Camera/CameraController.h"
#include "Game/Scene/SceneGameSelect.h"

// コンストラクタ
GS0_OniGokko::GS0_OniGokko()
{
	model = std::make_unique<Model>("Data/Model/GameSelect/Onigokko.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.05f;

	opacity = 0;

	// 幅、高さ設定
	radius = 5.0f;
	height = 1.0f;

	collisionOffset.y = -1.5f;
}

// デストラクタ
GS0_OniGokko::~GS0_OniGokko()
{
	//delete model;
}

// 更新処理
void GS0_OniGokko::Update(float elapsedTime)
{
	position = CameraController::target;
	if (!CameraController::debugCamera)
		position.y = CameraController::target.y + 17.0f;

	// ステート毎の更新処理
	switch (state)
	{
	case State::Wait:
		UpdateWaitState(elapsedTime);
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
void GS0_OniGokko::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// HPなどの描画
void GS0_OniGokko::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void GS0_OniGokko::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	GameSelect::DrawDebugPrimitive();

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
void GS0_OniGokko::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// ターゲット位置をランダム設定
void GS0_OniGokko::SetRandomTargetPosition()
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
void GS0_OniGokko::MoveToTarget(float elapsedTime, float speedRate)
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

// 待機ステートへ遷移
void GS0_OniGokko::TransitionWaitState()
{
	state = State::Wait;

	// タイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void GS0_OniGokko::UpdateWaitState(float elapsedTime)
{
	if (CameraController::debugCamera)
	{
		if (!CameraController::noMoveDebugCamera)
		{
			angle.y += DirectX::XMConvertToRadians(300) * elapsedTime;
			SetOpacityChange(1.0f, 2.0f);
		}
		else
		{
			SetOpacityChange(0.5f, 1.0f);
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 1), 0.5f, 1.0f);
		}
	}
	else
		SetOpacityChange(0.0f, 2.0f);
}

// プレイヤーとの接触処理
void GS0_OniGokko::CollisionVsPlayer()
{
	Player0_Onigokko& player = Player0_Onigokko::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (
		Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition))
	{
		//! シーンを切り替える
		SceneGameSelect::gameSelect = SceneGameSelect::GameSelect::Onigokko;
		SceneGameSelect::Instance().SetSceneChange();
	}
}