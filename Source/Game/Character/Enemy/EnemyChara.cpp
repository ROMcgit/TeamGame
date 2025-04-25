#include "EnemyChara.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G4_OssanTataki.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <algorithm>
#include "EnemyDarumasangaKoronda.h"

#include <random>

// コンストラクタ
EnemyChara::EnemyChara()
{
	models[0] = std::make_unique<Model>("Data/Model/4.OssanTataki/Ossan/Ossan.mdl");
	models[1] = std::make_unique<Model>("Data/Model/4.OssanTataki/Girl/Girl.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.15f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 2.9f;
	height = 25.5f;

	opacity = 0;

	// ムービー上昇ステートへ遷移
	TransitionMovieUpState();
}

// デストラクタ
EnemyChara::~EnemyChara()
{
	//delete model;
}

// 更新処理
void EnemyChara::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	// ムービー上昇
	case State::MovieUp:
		UpdateMovieUpState(elapsedTime);
		break;
	// ムービー下降
	case State::MovieDown:
		UpdateMovieDownState(elapsedTime);
		break;
	// 上昇
	case State::Up:
		UpdateUpState(elapsedTime);
		break;
	// 下降
	case State::Down:
		UpdateDownState(elapsedTime);
		break;
	// ダメージ
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// キャラクターの状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// モデルアニメーション更新
	models[modelNum]->UpdateAnimation(elapsedTime);

	// モデル行列更新
	models[modelNum]->UpdateTransform(transform);
}

// 描画処理
void EnemyChara::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	if(opacity > 0.0f)
	shader->Draw(dc, models[modelNum].get(), materialColor, opacity);
}

// HPなどの描画
void EnemyChara::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void EnemyChara::DrawDebugPrimitive()
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

void EnemyChara::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyChara"))
	{
		ImGui::InputFloat3("Velocity", &velocity.x);

		ImGui::InputFloat("StateChangeWaitTimer", &stateChangeWaitTimer);

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
void EnemyChara::OnDamaged()
{
}

// 死亡しと時に呼ばれる
void EnemyChara::OnDead()
{
}

// ムービー上昇ステートへ遷移
void EnemyChara::TransitionMovieUpState()
{
	state = State::MovieUp;

	//! 位置Yを変更する
	SetPositionYChange(21.5f, 1.0f);

	//! 不透明度を変更する
	SetOpacityChange(1.0f, 0.5f);

	stateChangeWaitTimer = 2.0f;
}

// ムービー上昇ステート更新処理
void EnemyChara::UpdateMovieUpState(float elapsedTime)
{
	if (!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		//! ステート変更の待ち時間が終わったら
		if (stateChangeWaitTimer <= 0.0f)
			TransitionMovieDownState();
	}
}

// ムービーダウンステートへ遷移
void EnemyChara::TransitionMovieDownState()
{
	state = State::MovieDown;

	//! 位置Yを変更する
	SetPositionYChange(-4.5f, 1.0f);

	//! 不透明度を変更する
	SetOpacityChange(0.0f, 0.5f);

	stateChangeWaitTimer = 4.0f;
}

// ムービーダウンステート更新処理
void EnemyChara::UpdateMovieDownState(float elapsedTime)
{
	if (!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		//! ステート変更の待ち時間が終わったら
		if (stateChangeWaitTimer <= 0.0f)
			TransitionUpState();
	}
}

// 上昇ステートへ遷移
void EnemyChara::TransitionUpState()
{
	state = State::Up;

	//! 位置Yを変更する
	SetPositionYChange(21.5f, 1.0f);

	//! 不透明度を変更する
	SetOpacityChange(1.0f, 0.5f);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1.3f, 3.0f);

	stateChangeWaitTimer = dist(gen);
}

// 上昇ステート更新処理
void EnemyChara::UpdateUpState(float elapsedTime)
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
void EnemyChara::TransitionDownState()
{
	state = State::Down;

	//! 位置Yを変更する
	SetPositionYChange(-4.5f, 1.0f);

	//! 不透明度を変更する
	SetOpacityChange(0.0f, 0.5f);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0.8f, 2.6f);

	stateChangeWaitTimer = dist(gen);
}

// 下降ステートへ遷移
void EnemyChara::UpdateDownState(float elapsedTime)
{
	//! 位置Yの変更が終わったら
	if (!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		if(stateChangeWaitTimer <= 0.0f)
		{
			modelNum = rand() % 2;

			//! 上昇ステートへ遷移
			TransitionUpState();
		}
	}
}

// ダメージステートへ遷移
void EnemyChara::TransitionDamageState()
{
	state = State::Damage;

	positionChange.y = false;

	stateChangeWaitTimer = 1.0f;
}

// ダメージステート更新処理
void EnemyChara::UpdateDamageState(float elapsedTime)
{
	if (!models[modelNum]->IsPlayAnimation())
	{
		//! 下降ステートへ遷移
		TransitionDownState();
	}
}