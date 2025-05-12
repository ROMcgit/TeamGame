#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// 目
class EnemyEye : public Enemy
{
public:
	EnemyEye();
	~EnemyEye() override;

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

	// プレイヤーを探す
	bool SearchPlayer();

	// 移動位置に移動
	void MoveTarget(float elapsedTime, float speedRate);

	// 待機ステートへ遷移
	void TransitionWaitState();

	// 待機ステート更新処理
	void UpdateWaitState(float elapsedTime);

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		Wait,
	};

private:
	std::unique_ptr<Model> model;

	bool deathState = false;

	bool setMoveTarget = false;

	bool playAnimation = false;
	float dist = 0;
	DirectX::XMFLOAT3 moveTarget = { 0, 0, 0 }; // 位置移動のターゲット(プレイヤーとは別)

	State state = State::Wait;
	float territoryRange = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 12.0f;
	float attackRange = 1.5f;

	bool rotationR = false;
};