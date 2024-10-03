#include "EnemySika.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player.h"
#include "Other/Collision.h"

// コンストラクタ
EnemySika::EnemySika()
{
	model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	gravity = 0.0f;

	// 幅、高さ設定
	radius = 0.5f;
	height = 1.0f;

	enemyHp = std::make_unique<Sprite>();

	// 追跡ステート
	TransitionPursuitState();
}

// デストラクタ
EnemySika::~EnemySika()
{
}

// 更新処理
void EnemySika::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Pursuit:
		UpdatePursuitState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	case State::IdleBattle:
		UpdateIdleBattleState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void EnemySika::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void EnemySika::SpriteRender(ID3D11DeviceContext* dc)
{

}

// HP表示
void EnemySika::RenderEnemyGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// 敵の位置を取得
	DirectX::XMFLOAT3 enemyPos = position;

	// 位置を変換
	DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&enemyPos);
	DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project(
		Pos,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World);

	// スクリーン座標を取得
	DirectX::XMFLOAT3 screenPos;
	DirectX::XMStoreFloat3(&screenPos, ScreenPos);

	// HPゲージの描画位置
	float gaugeWidth = health * 8; // HPゲージの幅
	float gaugeHeight = 5.0f;      // HPゲージの高さ
	float gaugeX = screenPos.x - gaugeWidth / 2;
	float gaugeY = screenPos.y - gaugeHeight - 55.0f; // 少し上にオフセット

	DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Unproject(
		ScreenPos,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	DirectX::XMFLOAT3 worldPos;
	DirectX::XMStoreFloat3(&worldPos, WorldPosition);

	enemyHp->Render(dc,
		gaugeX, //dx
		gaugeY, //dy
		gaugeWidth, //dw
		gaugeHeight, //dh
		0,           //sx
		0,           //sy
		gaugeWidth,  //sw
		gaugeHeight, //sh
		0,			 //angle
		1, 0, 0, 1   //color
	);
}

// デバッグプリミティブ描画
void EnemySika::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	// 縄張り範囲をデバッグ円柱描画
	debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
		DirectX::XMFLOAT4(0, 1, 0, 1));

	// ターゲット位置をデバッグ球描画
	debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	// 索敵範囲をデバッグ円柱描画
	debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	// 攻撃範囲をデバッグ円柱描画
	debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

// 縄張り設定
void EnemySika::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// ターゲット位置をランダム設定
void EnemySika::SetRandomTargetPosition()
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
void EnemySika::MoveToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// 追跡ステートへ遷移
void EnemySika::TransitionPursuitState()
{
	state = State::Pursuit;

	// 数秒間追跡するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// 歩きアニメーション再生
	model->PlayAnimation(Anim_RunFWD, true);
}

// 追跡ステート更新処理
void EnemySika::UpdatePursuitState(float elapsedTime)
{
	// 目標地点をプレイヤー位置に設定
	targetPosition = Player::Instance().GetPosition();

	// 目標地点へ移動
	MoveToTarget(elapsedTime, 10.0f);
}

// ノードとプレイヤーの衝突処理
void EnemySika::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	// ノードの位置と当たり判定を行う
	Model::Node* node = model->FindNode(nodeName);
	if (node != nullptr)
	{
		// ノードのワールド座標
		DirectX::XMFLOAT3 nodePosition(
			node->worldTransform._41,
			node->worldTransform._42,
			node->worldTransform._43
		);

		// 当たり判定表示
		Graphics::Instance().GetDebugRenderer()->DrawSphere(
			nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
		);

		// プレイヤーと当たり判定
		Player& player = Player::Instance();
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition))
		{
			// ダメージを与える
			if (player.ApplyDamage(1, 0.5f))
			{
				// 敵を吹っ飛ばすベクトルを算出
				DirectX::XMFLOAT3 vec;
				vec.x = outPosition.x - nodePosition.x;
				vec.z = outPosition.z - nodePosition.z;
				float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
				vec.x /= length;
				vec.z /= length;

				// XZ平面に吹っ飛ばす力をかける
				float power = 10.0f;
				vec.x *= power;
				vec.z *= power;
				// Y方向にも力をかける
				vec.y = 5.0f;

				// 吹っ飛ばす
				player.AddImpulse(vec);
			}
		}
	}
}

// 攻撃ステートへ遷移
void EnemySika::TransitionAttackState()
{
	state = State::Attack;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack1, false);
}

// 攻撃ステート更新処理
void EnemySika::UpdateAttackState(float elapsedTime)
{
	// 任意のアニメーション再生区間でのみ衝突判定処理をする
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.1f && animationTime <= 0.35f)
	{
		// 目玉ノードとプレイヤーの衝突処理
		CollisionNodeVsPlayer("EyeBall", 0.2f);
	}

	// 攻撃アニメーションが終わったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionIdleBattleState();
	}
}

// 戦闘待機ステートへ遷移
void EnemySika::TransitionIdleBattleState()
{
	state = State::IdleBattle;

	// 数秒間待機するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(2.0f, 3.0f);

	// 戦闘待機アニメーション再生
	model->PlayAnimation(Anim_IdleBattle, true);
}

// 戦闘待機ステート更新処理
void EnemySika::UpdateIdleBattleState(float elapsedTime)
{
	// 目標地点をプレイヤー位置に設定
	targetPosition = Player::Instance().GetPosition();

	// タイマー処理
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		// プレイヤー攻撃範囲にいた場合は攻撃ステートへ遷移
		float vx = targetPosition.x - position.x;
		float vy = targetPosition.y - position.y;
		float vz = targetPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
		if (dist < attackRange)
		{
			// 攻撃ステートへ遷移
			TransitionAttackState();
		}

		MoveToTarget(elapsedTime, 0.0f);
	}
}

// ダメージステートへ遷移
void EnemySika::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_GetHit, false);
}

// ダメージステート更新処理
void EnemySika::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionIdleBattleState();
	}
}

// 死亡ステートへ遷移
void EnemySika::TransitionDeathState()
{
	state = State::Death;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Die, false);
}

// 死亡ステート更新処理
void EnemySika::UpdateDeathState(float elapsedTime)
{
	// ダメージアニメーションが終わったら自分を破棄
	if (!model->IsPlayAnimation())
	{
		Destroy();
	}
}

// ダメージ受けた時に呼ばれる
void EnemySika::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡しと時に呼ばれる
void EnemySika::OnDead()
{
	//Destroy();

	// 死亡ステートへ遷移
	TransitionDeathState();
}
