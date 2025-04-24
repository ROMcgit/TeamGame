#include "EnemyOssan.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <algorithm>
#include "EnemyDarumasangaKoronda.h"

#include <random>

// コンストラクタ
EnemyOssan::EnemyOssan()
{
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

	// 上昇ステートへ遷移
	TransitionUpState();
}

// デストラクタ
EnemyOssan::~EnemyOssan()
{
	//delete model;
}

// 更新処理
void EnemyOssan::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	// 上昇
	case State::Up:
		UpdateUpState(elapsedTime);
		break;
	case State::Down:
		UpdateDownState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// キャラクターの状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void EnemyOssan::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity);
}

// HPなどの描画
void EnemyOssan::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void EnemyOssan::DrawDebugPrimitive()
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

void EnemyOssan::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyOssan"))
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
		}

		ImGui::TreePop();
	}
}

// ダメージ受けた時に呼ばれる
void EnemyOssan::OnDamaged()
{
}

// 死亡しと時に呼ばれる
void EnemyOssan::OnDead()
{
}

// 上昇ステートへ遷移
void EnemyOssan::TransitionUpState()
{
	state = State::Up;

	//! 位置Yを変更する
	SetPositionYChange(21.5f, 1.0f);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0.5f, 2.5f);

	stateChangeWaitTimer = dist(gen);
}

// 上昇ステート更新処理
void EnemyOssan::UpdateUpState(float elapsedTime)
{
	//! 位置Yの変更が終わったら
	if(!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		if (stateChangeWaitTimer <= 0.0f)
		{
			//! 下降ステートへ遷移
			TransitionDownState();
		}
	}
}

// 下降ステートへ遷移
void EnemyOssan::TransitionDownState()
{
	state = State::Down;
}

// 下降ステートへ遷移
void EnemyOssan::UpdateDownState(float elapsedTime)
{

}

// ダメージステートへ遷移
void EnemyOssan::TransitionDamageState()
{
	state = State::Damage;
}

// ダメージステート更新処理
void EnemyOssan::UpdateDamageState(float elapsedTime)
{

}