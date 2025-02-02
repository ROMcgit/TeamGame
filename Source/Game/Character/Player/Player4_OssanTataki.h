#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"

// プレイヤー
class Player4_OssanTataki : public Character
{
public:
	Player4_OssanTataki();
	~Player4_OssanTataki() override;

	// インスタンス取得
	static Player4_OssanTataki& Instance();

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

	/*! 加速度 */

	// 加速度Xを設定
	void SetVelocityX(float velocityX) { this->velocity.x = velocityX; }

	// 加速度Yを設定
	void SetVelocityY(float velocityY) { this->velocity.y = velocityY; }

	// 加速度Zを設定
	void SetVelocityZ(float velocityZ) { this->velocity.z = velocityZ; }

	/*! 位置 */

	// 位置Xを設定
	void SetPositionX(float positionX) { this->position.x = positionX; }

	// 位置Yを設定
	void SetPositionY(float positionY) { this->position.y = positionY; }

	// 位置Zを設定
	void SetPositionZ(float positionZ) { this->position.z = positionZ; }

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

	/*! 当たり判定処理 */

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayer4_OssanTatakiVsEnemies();

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

private:
	std::unique_ptr<Model> model;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	std::unique_ptr<Effect> hitEffect;

	State state = State::Wait;

	float playerAnimeCount = 0.0f;

	float leftHandRadius = 0.4f;

	bool attackCollisionFlag = false;
};