#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"
#include "Graphics/Fade.h"
#include "Graphics/Text.h"
#include "Graphics/Sprite.h"

// プレイヤー
class Player2_Sundome : public Character
{
public:
	Player2_Sundome();
	~Player2_Sundome() override;

	// インスタンス取得
	static Player2_Sundome& Instance();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// HPなどのUI描画
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	//! ラウンドを取得
	int GetRound() { return round; }

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

/******************************************************************************************/

	/*! 行動制御 */

	// ムービー待機ステートへ遷移
	void TransitionMovieWaitState();

	// ムービー待機ステート更新処理
	void UpdateMovieWaitState(float elapsedTime);

	// 待機ステートへ遷移
	void TransitionWaitState();

	// 待機ステート更新処理
	void UpdateWaitState(float elapsedTime);

//-----------------------------------------------------------------//

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	// 戻るステートへ遷移
	void TransitionReturnState();

	// 戻るステート更新処理
	void UpdateReturnState(float elapsedTime);

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
		MovieWait,
		Wait,
		Move,
		Return,
		Jump,
		Damage,
		Death
	};

private:
	std::unique_ptr<Model> models[3];
	std::unique_ptr<Fade> fade;

	std::unique_ptr<Sprite> actionExplanation[2];

	bool isBrake = false;
	bool breakeSound = false;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	State state = State::Wait;

	std::unique_ptr<Sprite> velocitySprite;               // 加速度のメーター
	std::unique_ptr<Sprite> velocitySpriteLimit;          // 加速度の最大値
	DirectX::XMFLOAT2 spritePos   = { 1170.0f, 640.0f };  // 加速度の画像の位置
	DirectX::XMFLOAT2 spriteScale = { 76.0f, -400.0f };   // 加速度の画像の大きさ

	std::unique_ptr<Text> velocityText;                    // 加速度の数値
	std::unique_ptr<Text> velocityTextSyosuten;            // 加速度の数値(小数点)
	DirectX::XMFLOAT2 velocityTextPos = { 1150.0f, 0.0f }; // 加速度の数値の位置

	//! 加速度の限界値
	struct VelocityLimit
	{
		float min = 0.0f;
		float max = 60.0f;
	}velocityLimit;

	bool velocityDown = false; // 加速度を下げるか
	float setVelocityX = 0.0f; // 加速度の設定

	float brake       = 0;   // ブレーキ
	int round        = 1;    // ラウンド
	float boundTimer = 0.0f; // バウンドの時間
};