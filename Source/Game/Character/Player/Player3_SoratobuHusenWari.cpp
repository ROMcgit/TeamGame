#include <imgui.h>
#include "Player3_SoratobuHusenWari.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Camera/CameraController.h"

static Player3_SoratobuHusenWari* instance = nullptr;

// インスタンス取得
Player3_SoratobuHusenWari& Player3_SoratobuHusenWari::Instance()
{
	return *instance;
}

Player3_SoratobuHusenWari::Player3_SoratobuHusenWari()
{
	// インスタンスポインタ設定
	instance = this;

	// モデル読み込み
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.6f;
	height = 5.0f;

	// 重力
	gravity = 0.3f;

	// 移動ステートへ遷移
	TransitionMoveState();
}

// デストラクタ
Player3_SoratobuHusenWari::~Player3_SoratobuHusenWari()
{
}

// 更新処理
void Player3_SoratobuHusenWari::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	// ムービー中なら待機ステートへ遷移
	if (movieScene)
	{
		// 全ての弾を破棄する
		int projectileCount = projectileManager.GetProjectileCount();
		for (int i = 0; i < projectileCount; ++i)
		{
			Projectile* projectile = projectileManager.GetProjectile(i);

			// 弾破棄
			projectile->Destroy();
		}

		// ムービー中のアニメーション
		if (!movieAnimation)
		{
			state = State::Move; // ステートを待機に変更
			model->PlayAnimation(movieAnimNum, movieAnimLoop);
			movieAnimation = true;
		}
	}
	// ムービー中では無い時
	else
		movieAnimation = false; // ムービー中に待機ステートかどうか

	if (!movieScene)
	{
		// ステート毎の処理
		switch (state)
		{
		case State::Move:
			UpdateMoveState(elapsedTime);
			break;
		case State::Damage:
			UpdateDamageState(elapsedTime);
			break;
		case State::Death:
			UpdateDeathState(elapsedTime);
			break;
		}
	}

	// キャラクター状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayer3_SoratobuHusenWariVsEnemies();

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// モデル更新処理
	model->UpdateTransform(transform);
}

// 描画処理
void Player3_SoratobuHusenWari::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());

	// 弾丸描画処理
	projectileManager.Render(dc, shader);
}

// HPなどのUI描画
void Player3_SoratobuHusenWari::SpriteRender(ID3D11DeviceContext* dc)
{
}

// 移動入力処理
bool Player3_SoratobuHusenWari::InputMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 移動処理
	Move3D(moveVec.x, moveVec.z, moveSpeed);

	// 旋回処理
	Turn3D_Player(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return !(moveVec.x == 0.0f && moveVec.z == 0.0f);
}

// 移動ステートへ遷移
void Player3_SoratobuHusenWari::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	model->PlayAnimation(Anim_Move, true);
}

// 移動ステート更新処理
void Player3_SoratobuHusenWari::UpdateMoveState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	float move = 5;
	float angleZ = 25;
	float angleZChangeTime = 0.2f;
	if ((gamePad.GetButtonHeld() & GamePad::BTN_LEFT) && !(gamePad.GetButtonHeld() & GamePad::BTN_RIGHT))
	{
#if 1
		velocity.x = -move;

		//! 角度の変更を解除
		SetAngleChangeUnlock();

		//! 角度Zを変更する
		SetAngleZChange(DirectX::XMConvertToRadians(-angleZ), angleZChangeTime);
#endif
	}
	else if ((gamePad.GetButtonHeld() & GamePad::BTN_RIGHT) && !(gamePad.GetButtonHeld() & GamePad::BTN_LEFT))
	{
#if 1
		velocity.x = move;

		//! 角度の変更を解除
		SetAngleChangeUnlock();

		//! 角度Zを変更する
		SetAngleZChange(DirectX::XMConvertToRadians(angleZ), angleZChangeTime);
#endif
	}
	else
	{
#if 1
		velocity.x = 0;

		//! 角度変更を解除
		SetAngleChangeUnlock();

		//! 角度Zを変更する
		SetAngleZChange(DirectX::XMConvertToRadians(0), angleZChangeTime);
#endif
	}

	GamePadButton button =
	GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;
	if (gamePad.GetButtonHeld() & button)
	{
		velocity.y += 20 * elapsedTime;
	}

	velocity.y = std::clamp(velocity.y, -5.0f, 5.0f);
}

// ダメージステートへ遷移
void Player3_SoratobuHusenWari::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Damage, false);
}

// ダメージステート更新処理
void Player3_SoratobuHusenWari::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionMoveState();
	}
}

// 死亡ステートへ遷移
void Player3_SoratobuHusenWari::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player3_SoratobuHusenWari::UpdateDeathState(float elapsedTimae)
{
}

// プレイヤーとエネミーとの衝突処理
void Player3_SoratobuHusenWari::CollisionPlayer3_SoratobuHusenWariVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		//if (Collision::IntersectSphereVsSphere(
		//	Player3_SoratobuHusenWari::GetPosition(),
		//	Player3_SoratobuHusenWari::GetRadius(),
		//	enemy->GetPosition(),
		//	enemy->GetRadius(),
		//	outPosition
		//))
		//{
		//	// 押し出しの後の位置設定
		//	enemy->SetPosition(outPosition);
		//}


		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
			//// プレイヤーが敵の上にいるかを判定する
			//float diff = Player3_SoratobuHusenWari::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player3_SoratobuHusenWari::Jump(10);
			//	// 小ジャンプさせるためにY方向の速度を設定する
			//}

			//// 押し出しの後の位置設定
			//enemy->SetPosition(outPosition);

			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			// 上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				// 小ジャンプする
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				// 押し出し後の位置設定
				enemy->SetPosition(outPosition);
			}

		}

	}
}

//デバッグプリミティブ描画
void Player3_SoratobuHusenWari::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// 着地した時に呼ばれる
void Player3_SoratobuHusenWari::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player3_SoratobuHusenWari::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player3_SoratobuHusenWari::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// 当たり判定描画
void Player3_SoratobuHusenWari::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player3_SoratobuHusenWari"))
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
		if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("Radius", &radius, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
		}
		ImGui::TreePop();
	}
}