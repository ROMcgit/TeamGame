#pragma once
#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "ProjectileManager.h"
#include "Effect.h"
#include "Audio/Audio.h"

// スライム
class EnemyStrongLoading : public Enemy
{
public:
	EnemyStrongLoading();
	~EnemyStrongLoading() override;

	// 唯一のインスタンス取得
	static EnemyStrongLoading& Instance()
	{
		static EnemyStrongLoading instace;
		return instace;
	}

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// ローディング画面
	void TransitionLoadingState();

	// ローディング画面
	void UpdateLoadingState();

private:
	// ステート
	enum class State
	{
		Wander,
		Idle,
		Pursuit,
		Attack,
		IdleBattle,
		Damage,
		Death,
		Loading
	};

	// アニメーション
	enum Animation
	{
		Anim_IdleNormal,
		Anim_IdleBattle,
		Anim_Attack1,
		Anim_Attack2,
		Anim_WalkFWD,
		Anim_WalkBWD,
		Anim_WalkLeft,
		Anim_WalkRight,
		Anim_RunFWD,
		Anim_SenseSomthingST,
		Anim_SenseSomthingPRT,
		Anim_Taunt,
		Anim_Victory,
		Anim_GetHit,
		Anim_Dizzy,
		Anim_Die
	};

private:
	std::unique_ptr<Model> model;

	State state = State::Wander;
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

	float posWave = 0.027f;
};