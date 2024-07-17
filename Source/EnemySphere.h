#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "ProjectileManager.h"
#include "Effect.h"
#include "Audio/Audio.h"

// スライム
class EnemySphere : public Enemy
{
public:
	EnemySphere();
	~EnemySphere() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	// 縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

protected:
	//ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// 位置調整
	void PositionControll();

	// 弾とプレイヤーの当たり判定
	void CollisionProjectilesVsPlayer();

	// 弾と敵の当たり判定
	void CollisionProjectilesVsEnemy();

	// 弾と壁の当たり判定
	void CollisionProjectilesVsWall();

	// ターゲット位置をランダム設定
	void SetRandomTargetPosition();

	// 目標地点へ移動
	void MoveToTarget(float elapsedTime, float speedRate);

	// 待機ステートへ遷移
	void TransitionIdleState();

	// 待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	// プレイヤー索敵
	//bool SearchPlayer();

	// ノードとプレイヤーの衝突処理
	void CollisionNodeVsPlayer(const char* nodeName, float nodeRadius);

	// 攻撃ステートへ遷移
	void TransitionAttackState();

	// 攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	// ダメージステートへ遷移
	void TransitionDamageState();

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		Idle,
		Attack,
		Damage,
		Death,
	};

	// アニメーション
	enum Animation
	{
		Anim_Idle,
		Anim_Attack,
		Anim_Damage,
		Anim_Death,
	};

private:
	std::unique_ptr<Model> model;

	State state = State::Idle;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;

	float positionRandamuL = 0;
	float positionRandamuR = 0;

	int waitL = 0;
	int waitR = 0;

	bool moveR = false;

	ProjectileManager projectileManager;

	int damageWaitTime = 0;
	int attackWait = 0;
	bool hansya = false;

	std::unique_ptr<Effect> hitEffect;
	std::unique_ptr<Effect> bariaEffect;

	std::unique_ptr<AudioSource> sound[4];
	std::unique_ptr<AudioSource> attackSound;
};