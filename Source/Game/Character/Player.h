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

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

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

	// ノードとエネミーの衝突処理
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	bool InputJump();

	// 弾丸入力処理
	void InputProjectile();

	// 攻撃入力処理
	bool InputAttack();

//-------------------------------------------------------------------------------------//

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	// 突進ステートへ遷移
	void TransitionLungesState();

	// 突進ステート更新
	void UpdateLungesState(float elapsedTime);

	// 攻撃ステートへ遷移
	void TransitionAttackState();

	// 攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	// ダメージステートへ遷移
	void TransitionDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTimae);

private:

	enum class State
	{
		Move,
		Lunges,
		Attack,
		Damage,
		Death,
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
	float turnSpeed = DirectX::XMConvertToRadians(120);

	float minAngleX = DirectX::XMConvertToRadians(-45); // 角度最小値
	float maxAngleX = DirectX::XMConvertToRadians(45);  // 角度最大値

	bool  lunges            = false; // 突進するか
	float lungesChargeTimer = 0.0f;  // 突進チャージ時間
	float lungesTimer       = 0.0f;  // 突進時間

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	std::unique_ptr<Effect> hitEffect;

	State state = State::Move;

	float playerAnimeCount = 0.0f;

	float leftHandRadius = 0.4f;

	bool attackCollisionFlag = false;
};