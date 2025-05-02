#include "EnemyDarumasangaKoronda.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player1_DarumasangaKoronda.h"
#include "Other/Collision.h"
#include "Game/Scene/G1_DarumasangaKoronda.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <random>
#include <algorithm>

// コンストラクタ
EnemyDarumasangaKoronda::EnemyDarumasangaKoronda()
{
	model = std::make_unique<Model>("Data/Model/0.Onigokko/Oni/Oni.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	gravity = 0.3f;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.6f;
	height = 5.0f;

	opacity = 0;
	SetOpacityChange(1.0f, 0.8f);

	TransitionEntryState();
}

// デストラクタ
EnemyDarumasangaKoronda::~EnemyDarumasangaKoronda()
{
	//delete model;
}

// 更新処理
void EnemyDarumasangaKoronda::Update(float elapsedTime)
{
	if (Timer::GetTimeM_Int() <= 0 && Timer::GetTimeS_Int() <= 0 && !deathState)
	{
		TransitionDeathState();
		deathState = true;
	}

	if (CollisionVsPlayer() && !deathState)
	{
		//! ムービーにする
		G1_DarumasangaKoronda::movieScene = true;

		deathState = true;

		//! 死亡ステートへ遷移
		TransitionDeathState();
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
		// 見る
	case State::Look:
		UpdateLookState(elapsedTime);
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
void EnemyDarumasangaKoronda::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player1_DarumasangaKoronda::Instance().GetPosition();
	dist = abs(targetPosition.x - position.x);
	if ((dist < 180 || G1_DarumasangaKoronda::movieScene))
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// HPなどの描画
void EnemyDarumasangaKoronda::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void EnemyDarumasangaKoronda::DrawDebugPrimitive()
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

void EnemyDarumasangaKoronda::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyDarumasangaKoronda"))
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
void EnemyDarumasangaKoronda::TransitionEntryState()
{
	state = State::Entry;

	stateChangeWaitTimer = 1.0f;

	model->PlayAnimation(Anim_Entry, false);
}

// 登場ステート更新処理
void EnemyDarumasangaKoronda::UpdateEntryState(float elapsedTime)
{
	if (!G1_DarumasangaKoronda::movieScene)
	{
		stateChangeWaitTimer -= elapsedTime;

		if(stateChangeWaitTimer <= 0.0f)
			TransitionWaitState();
	}
}

// 待機ステートへ遷移
void EnemyDarumasangaKoronda::TransitionWaitState()
{
	state = State::Wait;

	//! 角度Yを変更する
	SetAngleYChange(DirectX::XMConvertToRadians(0), 0.5f);
	
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<> dist(5.0f, 8.0f);
	float time = dist(gen);
	
	//! ステート切り替えまでの待ち時間
	stateChangeWaitTimer = time;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void EnemyDarumasangaKoronda::UpdateWaitState(float elapsedTime)
{
	//! ムービーシーンでないなら、待ち時間を減らす
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		//! 見るステートへ遷移
		TransitionLookState();
	}
	else if (stateChangeWaitTimer < 1.3f)
	{
		//! カラーフィルターを変更する
		DirectX::XMFLOAT3 color = Camera::postEffect.colorFilter;
		color.x += 1.5f;
		SetColorFilterChange(color, 1.3f);
	}
}

// 見るステート
void EnemyDarumasangaKoronda::TransitionLookState()
{
	state = State::Look;

	//! 角度Yを変更する
	SetAngleYChange(DirectX::XMConvertToRadians(180), 0.3f);

	//! コントラスト
	SetContrast(Camera::postEffect.contrast + 0.05f);

	//! 色収差
	SetChromaticAberration(Camera::postEffect.chromaticAberration + 0.05f);

	stateChangeWaitTimer = 1.8f;
}

// 見るステート更新処理
void EnemyDarumasangaKoronda::UpdateLookState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		//! カラーフィルターを戻す
		SetColorFilterResetChange(0.3f);

		//! コントラストを戻す
		SetContrastResetChange(0.3f);

		//! 色収差を戻す
		SetChromaticAberrationResetChange(0.3f);

		//! 待機ステートへ遷移
		TransitionWaitState();
	}
}

// 攻撃ステートへ遷移
void EnemyDarumasangaKoronda::TransitionAttackState()
{
	state = State::Attack;
	
	//! コントラスト
	SetContrastChange(1.5f, 0.5f);

	//! サチュレーション
	SetSaturationChange(1.0f, 0.5f);

	//! カラーフィルター
	SetColorFilterChange(DirectX::XMFLOAT3(3.0f, 1.3f, 1.35f), 0.5f);

	//! クロマティックアベレーション
	SetChromaticAberrationChange(0.03f, 1.5f);

	stateChangeWaitTimer = 2.2f;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack, false);
}

// 攻撃ステート更新処理
void EnemyDarumasangaKoronda::UpdateAttackState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		//! ポストエフェクトを元に戻す
		SetPostEffectStatusResetChange();

		TransitionWaitState();
	}
}

// 死亡ステートへ遷移
void EnemyDarumasangaKoronda::TransitionDeathState()
{
	state = State::Death;

	angle.y = DirectX::XMConvertToRadians(180);

	stateChangeWaitTimer = 2.0f;
}

// 死亡ステート更新処理
void EnemyDarumasangaKoronda::UpdateDeathState(float elapsedTime)
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
void EnemyDarumasangaKoronda::OnDamaged()
{
}

// 死亡しと時に呼ばれる
void EnemyDarumasangaKoronda::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// プレイヤーとの当たり判定
bool EnemyDarumasangaKoronda::CollisionVsPlayer()
{
	Player1_DarumasangaKoronda& player = Player1_DarumasangaKoronda::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		collisionPos,
		radius,
		height,
		player.GetCollisionPos(),
		player.GetRadius(),
		player.GetHeight(), outPosition))
	{
		return true;
	}

	return false;
}

// 移動位置に移動
void EnemyDarumasangaKoronda::MoveTarget(float elapsedTime, float speedRate)
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
