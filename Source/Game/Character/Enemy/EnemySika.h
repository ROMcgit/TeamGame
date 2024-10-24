#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// スライム
class EnemySika : public Enemy
{
public:
	EnemySika();
	~EnemySika() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HPなどの描画
	void SpriteRender(ID3D11DeviceContext* dc) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

protected:
	//ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// プレイヤーとの接触処理
	void CollisionEnemyVsPlayer();

	// ノードとプレイヤーの衝突処理
	void CollisionNodeVsPlayer(const char* nodeName, float nodeRadius);

/***********************************************************************************************/

	/*! 行動制御 */

	// 追跡ステートへ遷移
	void TransitionPursuitState();

	// 追跡ステート更新処理
	void UpdatePursuitState(float elapsedTime);

//-----------------------------------------------------------------//

	// ダメージステートへ遷移
	void TransitionDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

//-----------------------------------------------------------------//

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		Pursuit,
		Attack,
		IdleBattle,
		Damage,
		Death
	};

private:
	std::unique_ptr<Model> model;

	std::unique_ptr<Sprite> enemyHp;

	State state = State::Pursuit;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;

	bool setVelocity = false;

	float lifeTimer = 10.0f;
};