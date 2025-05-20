#include <imgui.h>
#include "Player4_OssanTataki.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Tataki.h"
#include "Game/Camera/CameraController.h"
#include "Audio/SoundManager.h"

static Player4_OssanTataki* instance = nullptr;

// インスタンス取得
Player4_OssanTataki& Player4_OssanTataki::Instance()
{
	return *instance;
}

Player4_OssanTataki::Player4_OssanTataki()
{
	// インスタンスポインタ設定
	instance = this;

	// モデル読み込み
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	effect = std::make_unique<Effect>("Data/Effect/Attack.efk");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	debugPrimitiveColor = { 0, 0, 1 };

	//! 衝突攻撃を生成
	std::unique_ptr<CollisionAttack_Tataki> tataki = std::make_unique<CollisionAttack_Tataki>(&collisionAttackManager);
	collisionAttackManager.Register(std::move(tataki));

	SoundManager::Instance().LoadSound("攻撃", "Data/Audio/Sound/Attack.wav");

	// 待機ステートへ遷移
	TransitionWaitState();
}

// デストラクタ
Player4_OssanTataki::~Player4_OssanTataki()
{
}

// 更新処理
void Player4_OssanTataki::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	if (!movieScene)
	{
		// ステート毎の処理
		switch (state)
		{
		case State::Wait:
			UpdateWaitState(elapsedTime);
			break;
		case State::Attack:
			UpdateAttackState(elapsedTime);
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

	// 衝突攻撃更新処理
	collisionAttackManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayer4_OssanTatakiVsEnemies();

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// モデル更新処理
	model->UpdateTransform(transform);
}

// 描画処理
void Player4_OssanTataki::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	//shader->Draw(dc, model.get());

	// 衝突攻撃描画処理
	collisionAttackManager.Render(dc, shader);
}

// HPなどのUI描画
void Player4_OssanTataki::SpriteRender(ID3D11DeviceContext* dc)
{
}

// 待機ステートへ遷移
void Player4_OssanTataki::TransitionWaitState()
{
	state = State::Wait;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void Player4_OssanTataki::UpdateWaitState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	CollisionAttack* tataki = collisionAttackManager.GetCollisionAttack(0);

	GamePadButton button = GamePad::BTN_UP | GamePad::BTN_DOWN | GamePad::BTN_LEFT | GamePad::BTN_RIGHT;

	int buttonState = gamePad.GetButtonDown();

	DirectX::XMFLOAT3 pos = { 10.0f, 21.5f, -19.0f };

	float offsetX = 32.0f;
	float offsetZ = 17.5f;

	switch (buttonState)
	{
	case GamePad::BTN_UP:    pos.z += offsetZ; break;
	case GamePad::BTN_DOWN:  pos.z -= offsetZ; break;
	case GamePad::BTN_LEFT:  pos.x -= offsetX; break;
	case GamePad::BTN_RIGHT: pos.x += offsetX; break;
	default:
		break;
	}

	tataki->SetPosition(pos);

	if (gamePad.GetButtonDown() & button)
	{
		DirectX::XMFLOAT3 e = pos;

		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for(int i = 0; i < enemyCount; i++)
		{
			std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);
			CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(0);

			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder
				(collisionAttack->GetPosition(),
					collisionAttack->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				e.y = enemy->GetPosition().y + enemy->GetHeight();
			}
		}

		effect->Play(e, { 1.5f,1.5f,1.5f });

		tataki->SetRadius(3.0f);

		//! 攻撃ステートへ遷移
		TransitionAttackState();
	}
}

// 攻撃ステートへ遷移
void Player4_OssanTataki::TransitionAttackState()
{
	state = State::Attack;

	SoundManager& sound = SoundManager::Instance();
	sound.StopSound("攻撃");
	sound.PlaySound("攻撃");

	stateChangeWaitTimer = 0.1f;
}

// 攻撃ステート更新処理
void Player4_OssanTataki::UpdateAttackState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		CollisionAttack* tataki = collisionAttackManager.GetCollisionAttack(0);
		tataki->SetRadius(0.0f);

		//! 待機ステートへ遷移
		TransitionWaitState();
	}
}

// ダメージステートへ遷移
void Player4_OssanTataki::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Damage, false);
}

// ダメージステート更新処理
void Player4_OssanTataki::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionWaitState();
	}
}

// 死亡ステートへ遷移
void Player4_OssanTataki::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player4_OssanTataki::UpdateDeathState(float elapsedTimae)
{
}

// プレイヤーとエネミーとの衝突処理
void Player4_OssanTataki::CollisionPlayer4_OssanTatakiVsEnemies()
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
		//	Player4_OssanTataki::GetPosition(),
		//	Player4_OssanTataki::GetRadius(),
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
			//float diff = Player4_OssanTataki::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player4_OssanTataki::Jump(10);
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
void Player4_OssanTataki::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// 衝突攻撃デバッグプリミティブ描画
	collisionAttackManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// 着地した時に呼ばれる
void Player4_OssanTataki::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player4_OssanTataki::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player4_OssanTataki::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// 当たり判定描画
void Player4_OssanTataki::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player4_OssanTataki"))
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