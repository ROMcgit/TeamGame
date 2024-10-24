#include "EnemySika.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player.h"
#include "Other/Collision.h"
#include "Game/Scene/SceneTitle.h"
#include "Audio/SoundEffectManager.h"

// コンストラクタ
EnemySika::EnemySika()
{
	hpDirectorFinished = true;

	model = std::make_unique<Model>("Data/Model/Sika/Sika.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.00f;

	// 大きさ変更
	SetScaleChange(DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f), DirectX::XMFLOAT3(0.4f, 0.4f, 0.4f));

	gravity = 0.0f;
	
	radius = 0.0f;
	height = 0.0f;

	collisionOffset = { 0, -1.0f, 0 };

	enemyHp = std::make_unique<Sprite>();

	Player& player = Player::Instance();

	hp = 1;

	SoundEffectManager::Instance().LoadSoundEffect("攻撃ヒット", "Data/Audio/攻撃ヒット.wav");

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
	if (scale.x >= 0.05f)
	{
		// 幅、高さ設定
		radius = 1.5f;
		height = 2.4f;
	}

	// ステート毎の更新処理
	switch (state)
	{
	case State::Pursuit:
		UpdatePursuitState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// 当たり判定の位置設定
	CollisionPosSettings();

	// スケール変更更新処理
	UpdateScaleChange(elapsedTime);

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// プレイヤーとの当たり判定
	CollisionEnemyVsPlayer();

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

// デバッグプリミティブ描画
void EnemySika::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	// ターゲット位置をデバッグ球描画
	debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));
}

// プレイヤーとの接触処理
void EnemySika::CollisionEnemyVsPlayer()
{
	Player& player = Player::Instance();

	// 衝突処理
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		position,
		radius,
		height,
		outPosition
	))
	{
		if (player.ApplyDamage(player.GetViewMoveSpeed() + 10, 1.0f))
		{
			// 前方向
			DirectX::XMFLOAT3 velocity;

			if (position.x > player.GetPosition().x)
				velocity.x = -80;
			else
				velocity.x = 80;

			if (position.z > player.GetPosition().z)
				velocity.z = -80;
			else
				velocity.z = 80;

			player.SetVelocity(DirectX::XMFLOAT3(velocity.x, 100.0f, velocity.z));
		}
		else
		{
			SoundEffectManager::Instance().StopSoundEffect("攻撃ヒット");
			SoundEffectManager::Instance().PlaySoundEffect("攻撃ヒット");

			hp = 0;
			TransitionDeathState(); // 死亡ステートへ遷移
		}
	}
}

// 追跡ステートへ遷移
void EnemySika::TransitionPursuitState()
{
	state = State::Pursuit;

	// 数秒間追跡するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	model->PlayAnimation(0, true);
}

// 追跡ステート更新処理
void EnemySika::UpdatePursuitState(float elapsedTime)
{
	if (position.y > 1.1)
	{
		velocity.y = -3;
	}
	else if (position.y < 0.9)
	{
		velocity.y = 3;
	}
	else if (position.y <= 1.05f && position.y >= 0.95f)
	{
		velocity.y = 0;
		position.y = 1;
	}
	
	

	SceneTitle& scene = SceneTitle::Instance();
	if (scene.gameClear)
		TransitionDeathState();

	// プレイヤーのインスタンス取得
	Player& player = Player::Instance();

	float vx = player.GetPosition().x - position.x;
	float vz = player.GetPosition().z - position.z;

	// 移動処理
	Move(vx, vz, 5.0f);
	Turn(elapsedTime, vx, vz, 50.0f);

	dist = vx * vx + vz * vz;
	if (dist > 2500)
		Destroy();

	lifeTimer -= elapsedTime;
	if (lifeTimer < 0.0f)
	{
		SetScaleChange(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f));

		if (scale.x <= 0)
			Destroy();
	}
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

// ダメージステートへ遷移
void EnemySika::TransitionDamageState()
{
	state = State::Damage;
}

// ダメージステート更新処理
void EnemySika::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionPursuitState();
	}
}

// 死亡ステートへ遷移
void EnemySika::TransitionDeathState()
{
	hp = 0;

	state = State::Death;

	Player& player = Player::Instance();

	player.SetExp(1);

	stateTimer = 1.0f;
}

// 死亡ステート更新処理
void EnemySika::UpdateDeathState(float elapsedTime)
{
	angle.x += DirectX::XMConvertToRadians(700) * elapsedTime;
	angle.y += DirectX::XMConvertToRadians(700) * elapsedTime;

	velocity.y = 40.0f;


	Player& player = Player::Instance();

	if (!setVelocity)
	{
		if (position.x > player.GetPosition().x)
			velocity.x = 40;
		else
			velocity.x = -40;

		if (position.z > player.GetPosition().z)
			velocity.z = 40;
		else
			velocity.z = -40;

		setVelocity = true;
	}
	

	// ダメージアニメーションが終わったら自分を破棄
	if (stateTimer <= 0)
	{
		Destroy();
	}

	stateTimer -= elapsedTime;
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