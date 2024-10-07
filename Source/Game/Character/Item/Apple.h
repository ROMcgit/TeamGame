#pragma once

#include "Graphics//Model.h"
#include "Item.h"
#include "Graphics/Graphics.h"
#include "Game/Effect/Effect.h"
#include "Other/Mathf.h"

// スライム
class Apple : public Item
{
public:
	Apple();
	~Apple() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HPなどの描画
	void SpriteRender(ID3D11DeviceContext* dc) override;

	// デバッグ用GUI描画
	void DrawDebugGUI() override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

private:

	// プレイヤーとの接触処理
	void CollisionItemVsPlayer();

/***********************************************************************************************/

	/*! 行動制御 */

	// 待機ステートへ遷移
	void TransitionInitState();

	// 待機ステート更新処理
	void UpdateInitState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		Init,
	};

private:
	std::unique_ptr<Model> model;
	std::unique_ptr<Sprite> enemyHp;
	std::unique_ptr<Effect> getEffect;

	State state = State::Init;
	DirectX::XMFLOAT3 targetPosition  = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;

	float stateTimer  = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;

	float dist = 0;

	float lifeTimer = 10.0f;
};