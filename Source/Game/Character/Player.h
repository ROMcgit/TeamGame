#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"

// プレイヤー
class Player : public Character
{
public:
	Player();
	~Player() override;

	// インスタンス取得
	static Player& Instance();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// HPなどのUI描画
	void SpriteRender(ID3D11DeviceContext* dc);

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

	// ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	bool InputJump();

	// 弾丸入力処理
	void InputProjectile();

	// 攻撃入力処理
	bool InputAttack();

	/*! 当たり判定処理 */

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	// ノードとエネミーの衝突処理
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	// 弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();

/******************************************************************************************/

	/*! 行動制御 */

	// 待機ステートへ遷移
	void TransitionIdleState();

	// 待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

//-----------------------------------------------------------------//

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

//-----------------------------------------------------------------//

	// ジャンプステートへ遷移
	void TransitionJumpState();

	// ジャンプステート更新処理
	void UpdateJumpState(float elapsedTime);

//-----------------------------------------------------------------//

	// 着地ステートへ遷移
	void TransitionLandState();

	// 着地ステート更新処理
	void UpdateLandState(float elapsedTime);

//-----------------------------------------------------------------//

	// 攻撃ステートへ遷移
	void TransitionAttackState();

	// 攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

//-----------------------------------------------------------------//

	// ダメージステートへ遷移
	void TransitionDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

//-----------------------------------------------------------------//

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTimae);

//-----------------------------------------------------------------//

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
	std::unique_ptr<Model> model;
	
	float moveSpeed = 5.5f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	std::unique_ptr<Effect> hitEffect;

	State state = State::Idle;

	float playerAnimeCount = 0.0f;

	float leftHandRadius = 0.4f;

	bool attackCollisionFlag = false;
};