#pragma once

#include "Graphics//Model.h"
#include "Obstacle.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// トゲマル
class Togemaru : public Obstacle
{
public:
	Togemaru();
	~Togemaru() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HPなどの描画
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

private:

	// プレイヤーとの当たり判定
	void CollisionVsPlayer();

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		Move,
	};

	// アニメーション
	enum Animation
	{
		Anim_Move,
	};

private:
	std::unique_ptr<Model> model;

	State state = State::Move;
	float territoryRange = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};