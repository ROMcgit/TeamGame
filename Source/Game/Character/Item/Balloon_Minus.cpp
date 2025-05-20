#include "Balloon_Minus.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"
#include "Audio/SoundManager.h"

#include <algorithm>

// コンストラクタ
Balloon_Minus::Balloon_Minus()
{
	model = std::make_unique<Model>("Data/Model/3.SoratobuHusenWari/Balloon_Minus/Balloon_Minus.mdl");

	//! エフェクト
	effect = std::make_unique<Effect>("Data/Effect/Minus.efk");

	// 位置Yのリセット
	positionResetY = position.y;

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.008f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.8f;
	height = 1.95f;

	collisionOffset.y = -0.55f;

	TransitionMoveState();
	
	SoundManager::Instance().LoadSound("破裂", "Data/Audio/Sound/Burst.wav");
}

// デストラクタ
Balloon_Minus::~Balloon_Minus()
{
	//delete model;
}

// 更新処理
void Balloon_Minus::Update(float elapsedTime)
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
void Balloon_Minus::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player3_SoratobuHusenWari::Instance().GetPosition();
	float vz = abs(targetPosition.z - position.z);
	if (vz < 80)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// デバッグプリミティブ描画
void Balloon_Minus::DrawDebugPrimitive()
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

void Balloon_Minus::DrawDebugGUI()
{
	if (ImGui::TreeNode("Balloon_Minus"))
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
void Balloon_Minus::TransitionMoveState()
{
	state = State::Move;
}

// 移動ステート更新処理
void Balloon_Minus::UpdateMoveState(float elapsedTime)
{
	if (G3_SoratobuHusenWari::gameTimer < 30.0f)
		velocity.z = -8.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
		velocity.z = -13.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
		velocity.z = -18.0f;
	else
		velocity.z = -23.0f;

	float posY = position.y == positionResetY ? positionResetY + 0.1f : positionResetY;
	SetPositionYChange(posY, 0.5f);

	//! プレイヤーと当たったら
	if (CollisionVsPlayer3())
		TransitionBreakState();

	if (position.z < -20.0f)
		Destroy();
}

// 死亡ステートへ遷移
void Balloon_Minus::TransitionBreakState()
{
	state = State::Break;

	DirectX::XMFLOAT3 e = position;
	e.y += height * 0.5f;

	effect->Play(e, { 1.2f, 1.2f, 1.2f });

	SoundManager::Instance().PlaySound("破裂");

	G3_SoratobuHusenWari::score -= 3;
}

// 死亡ステート更新処理
void Balloon_Minus::UpdateBreakState(float elapsedTime)
{
	Destroy();
}

// ダメージ受けた時に呼ばれる
void Balloon_Minus::OnDamaged()
{
}

// 死亡した時に呼ばれる
void Balloon_Minus::OnDead()
{
}