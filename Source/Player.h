#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

// プレイヤー
class Player : public Character
{
public:
	Player();
	~Player() override;

	// インスタンス取得
	static Player& Instance();

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();

protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

	// ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// 位置調整
	void PositionControll();

	// 移動入力処理
	//void InputMove(float elapsedTime);
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	//void InputJump();
	//bool InputJump();

	// 弾丸入力処理
	void InputProjectile();

	// 攻撃入力処理
	bool InputAttack();

	// 待機ステートへ遷移
	void TransitionIdleState();

	// 待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	//// ジャンプステートへ遷移
	//void TransitionJumpState();

	//// ジャンプステート更新処理
	//void UpdateJumpState(float elapsedTime);

	//// 着地ステートへ遷移
	//void TransitionLandState();

	//// 着地ステート更新処理
	//void UpdateLandState(float elapsedTime);

	// 攻撃ステートへ遷移
	void TransitionAttackState();

	// 攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	// ノードとエネミーの衝突処理
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	// ダメージステートへ遷移
	void TransitionDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTimae);

	// 復活ステートへ遷移
	void TransitionReviveState();

	// 復活ステート更新処理
	void UpdateReviveState(float elapsedTime);

private:

	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Attack,
		Damage,
		Death,
		Revive
	};

	// アニメーション
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_Jump,
		Anim_Jump_Flip,
		Anim_Landing,
		Anim_Reving,
		Anim_Running,
		Anim_Walking
	};

private:
	Model* model = nullptr;
	
	float moveSpeed = 15.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	Effect* hitEffect = nullptr;

	State state = State::Idle;

	int playerMove;
	int playerAnimeCount;

	float leftHandRadius = 0.4f;

	bool attackCollisionFlag = false;
};