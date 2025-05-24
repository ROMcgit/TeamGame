#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"
#include "Game/Character/CollisionAttack/CollisionAttackManager.h"

// プレイヤー
class Player3_SoratobuHusenWari : public Character
{
public:
	Player3_SoratobuHusenWari();
	~Player3_SoratobuHusenWari() override;

	// インスタンス取得
	static Player3_SoratobuHusenWari& Instance();

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
	// 位置制限
	void PositionControll(float elapsedTime);

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// HPピンチ更新処理
	void UpdateHpPinch(float elapsedTime);

	/*! 当たり判定処理 */

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
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
	void UpdateDeathState(float elapsedTimae);

private:

	enum class State
	{
		Move,
		Damage,
		Death,
	};

	// アニメーション
	enum Animation
	{
		Anim_Wait,
		Anim_Move,
		Anim_Jump,
		Anim_Damage,
		Anim_Death,
	};

public:
	bool isDamage = false;

private:
	std::unique_ptr<Model> model;

	std::unique_ptr<Sprite> uiSprite[4];

	float hpPinchTimer = 0.0f;

	// 衝突攻撃
	CollisionAttackManager collisionAttackManager;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	State state = State::Move;

	bool gameStart = false;
};