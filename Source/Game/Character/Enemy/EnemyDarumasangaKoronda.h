#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// 鬼
class EnemyDarumasangaKoronda : public Enemy
{
public:
	EnemyDarumasangaKoronda();
	~EnemyDarumasangaKoronda() override;

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

	// プレイヤーとの当たり判定
	bool CollisionVsPlayer();

	// 移動位置に移動
	void MoveTarget(float elapsedTime, float speedRate);

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

	// 見るステート
	void TransitionLookState();

	// 見るステート更新処理
	void UpdateLookState(float elapsedTime);

	// 攻撃ステートへ遷移
	void TransitionAttackState();

	// 攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

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
		Look,
		Attack,
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
		Anim_Death
	};

private:
	std::unique_ptr<Model> model;

	DirectX::XMFLOAT3 posReset = { 0.0f, 113.227f, 538.45f };
	
	bool deathState = false;

	bool noiseSoundPlay = false;

	bool setMoveTarget = false;

	bool playAnimation = false;
	float dist = 0;
	DirectX::XMFLOAT3 moveTarget = { 0, 0, 0 }; // 位置移動のターゲット(プレイヤーとは別)

	State state = State::Wait;
	float territoryRange = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};