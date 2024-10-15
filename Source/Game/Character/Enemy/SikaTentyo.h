#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// スライム
class SikaTentyo : public Enemy
{
public:
	SikaTentyo();
	~SikaTentyo() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HPなどの描画
	void SpriteRender(ID3D11DeviceContext* dc) override;

	// HP表示
	void RenderEnemyGauge(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

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

	// 登場ステートへ遷移
	void TransitionEntryState();

	// 登場ステート更新処理
	void UpdateEntryState(float elapsedTime);

	//-----------------------------------------------------------------//

	// 開始ステートへ遷移
	void TransitionStartState();

	// 開始ステート更新処理
	void UpdateStartState(float elapsedTime);

	//-----------------------------------------------------------------//

	// 開始ステートへ遷移
	void TransitionMoveState();

	// 開始ステート更新処理
	void UpdateMoveState(float elapsedTime);

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
		Entry,
		Start,
		Move,
		Damage,
		Death
	};

	// アニメーション
	enum Animation
	{
		Anim_Entry,
		Anim_Start,
		Anim_Move,
	};

private:
	std::unique_ptr<Model> model;

	std::unique_ptr<Sprite> enemyHp;

	State state = State::Entry;
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