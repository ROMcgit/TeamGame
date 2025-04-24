#include "Balloon_Plus.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <algorithm>

// コンストラクタ
Balloon_Plus::Balloon_Plus()
{
	model = std::make_unique<Model>("Data/Model/3.SoratobuHusenWari/Balloon_Plus/Balloon_Plus.mdl");

	// 位置Yのリセット
	positionResetY = position.y;

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.015f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 1.45f;
	height = 3.55f;

	collisionOffset.y = -0.95f;

	TransitionMoveState();
}

// デストラクタ
Balloon_Plus::~Balloon_Plus()
{
	//delete model;
}

// 更新処理
void Balloon_Plus::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	// 移動
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	// 壊れる
	case State::Break:
		UpdateBreakState(elapsedTime);
		break;
	}

	// キャラクターの状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void Balloon_Plus::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player3_SoratobuHusenWari::Instance().GetPosition();
	float vz = abs(targetPosition.z - position.z);
	if (vz < 80)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// デバッグプリミティブ描画
void Balloon_Plus::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Item::DrawDebugPrimitive();

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

void Balloon_Plus::DrawDebugGUI()
{
	if (ImGui::TreeNode("Balloon_Plus"))
	{
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

			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
		}

		ImGui::TreePop();
	}
}

// 移動ステートへ遷移
void Balloon_Plus::TransitionMoveState()
{
	state = State::Move;
}

// 移動ステート更新処理
void Balloon_Plus::UpdateMoveState(float elapsedTime)
{
	velocity.z = -8.0f;

	float posY = position.y == positionResetY ? positionResetY + 0.1f : positionResetY;
	SetPositionYChange(posY, 0.5f);

	//! プレイヤーと当たったら
	if (CollisionVsPlayer3() || position.z < -10.0f)
		TransitionBreakState();
}

// 死亡ステートへ遷移
void Balloon_Plus::TransitionBreakState()
{
	state = State::Break;

	G3_SoratobuHusenWari::score += 5;
}

// 死亡ステート更新処理
void Balloon_Plus::UpdateBreakState(float elapsedTime)
{
	Destroy();
}

// ダメージ受けた時に呼ばれる
void Balloon_Plus::OnDamaged()
{
}

// 死亡した時に呼ばれる
void Balloon_Plus::OnDead()
{
}