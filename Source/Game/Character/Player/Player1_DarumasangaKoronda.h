#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"

// プレイヤー
class Player1_DarumasangaKoronda : public Character
{
public:
	Player1_DarumasangaKoronda();
	~Player1_DarumasangaKoronda() override;

	// インスタンス取得
	static Player1_DarumasangaKoronda& Instance();

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

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	bool InputJump();

	/*! 当たり判定処理 */

/******************************************************************************************/

	/*! 行動制御 */

	// 待機ステートへ遷移
	void TransitionWaitState();

	// 待機ステート更新処理
	void UpdateWaitState(float elapsedTime);

//-----------------------------------------------------------------//

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

//-----------------------------------------------------------------//

	// ジャンプステートへ遷移
	void TransitionJumpState();

	// ジャンプステート更新処理
	void UpdateJumpState(float elapsedTime);

//-----------------------------------------------------------------//

	// ダッシュステートへ遷移
	void TransitionDashState();

	// ダッシュステート更新処理
	void UpdateDashState(float elapsedTime);

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
		Wait,
		Move,
		Jump,
		Dash,
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
	static bool isDamage;

private:
	std::unique_ptr<Model> model;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	bool setVelocityY = false; // 加速度の設定

	float jumpSpeed = 20.0f;

	State state = State::Wait;
};