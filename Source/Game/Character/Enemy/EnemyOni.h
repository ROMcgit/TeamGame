#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// 鬼
class EnemyOni : public Enemy
{
public:
	EnemyOni();
	~EnemyOni() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HPなどの描画
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	// デバッグGUI
	void DrawDebugGUI() override;

protected:
	//ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// ノードとプレイヤーの衝突処理
	//void CollisionNodeVsPlayer(const char* nodeName, float nodeRadius);

	// 登場ステートへ遷移
	void TransitionEntryState();

	// 登場ステート更新処理
	void UpdateEntryState(float elapsedTime);

	// 待機ステートへ遷移
	void TransitionWaitState();

	// 待機ステート更新処理
	void UpdateWaitState(float elapsedTime);

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	// 威嚇ステートへ遷移
	void TransitionLaughState();

	// 威嚇ステート更新処理
	void UpdateLaughState(float elapsedTime);

	// 追跡ステートへ遷移
	void TransitionTrackingState();

	// 追跡ステート更新処理
	void UpdateTrackingState(float elapsedTime);

	// 疲れるステート遷移
	void TransitionTiredState();

	// 疲れるステート更新処理
	void UpdateTiredState(float elapsedTime);

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
	void UpdateDeathState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		Entry,
		Wait,
		Move,
		Laugh,
		Tracking,
		Tired,
		Attack,
		Damage,
		Death
	};

	// アニメーション
	enum Animation
	{
		Anim_Entry,
		Anim_Wait,
		Anim_Move,
		Anim_Laugh,
		Anim_Tracking,
		Anim_Tired,
		Anim_Attack,
		Anim_Damage,
		Anim_Death
	};

private:
	std::unique_ptr<Model> model;

	float dist = 0;

	State state = State::Wait;
	float territoryRange = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};