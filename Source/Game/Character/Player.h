#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Character/Yazirusi/YazirusiManager.h"
#include "Game/Effect/Effect.h"
#include "Graphics/Text.h"

// プレイヤー
class Player : public Character
{
public:
	Player();
	~Player() override;

	// インスタンス取得
	static Player& Instance();

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

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

	// 弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();

	// アイテム取得設定
	void SetDiffusionAttacks(int diffusionAttacks)
	{
		if(this->diffusionAttacks < 5)
		this->diffusionAttacks += diffusionAttacks;
	}

	// HP回復
	void SetHpUp(int hpUp)
	{
		this->hp += hpUp;
	}

	// 無敵状態設定
	void SetInvincibleState(bool invincibleState)
	{
		this->invincibleState = invincibleState;
		invincibleTimer = 10.0f;
	}

	// 無敵状態取得
	bool GetInvincibleState() { return invincibleState; }

	// expを与える
	void SetExp(int exp)
	{
		this->exp += exp;
	}

	// レベルを取得
	int GetLevel() { return level; }

	// バナナの数を増やす
	void SetBananaNum(float bananaNum)
	{
		this->bananaNum += bananaNum;
	}

	// バナナの習得数取得
	int GetBananaNum() { return bananaNum; }

	// 移動スピードを取得
	float GetMoveSpeed() { return moveSpeed; }

	// 表示スピードを取得
	int GetViewMoveSpeed() { return viewMoveSpeed; }

	// 突進中かを取得
	bool GetLunges() { return lunges; }

	// 突進カウント設定
	void SetLungesCount(int lungesCount)
	{
		this->lungesCount = lungesCount;
	}

	// 突進カウント
	int GetLungesCount() { return lungesCount; }

protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

	// ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// ノードとエネミーの衝突処理
	//void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	// レベル更新処理
	void UpdateLevel();

	// 移動入力処理
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	bool InputJump();

	// 弾丸入力処理
	void InputProjectile();

	// 攻撃入力処理
	bool InputAttack();

//-------------------------------------------------------------------------------------//

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	// 突進ステートへ遷移
	void TransitionLungesState();

	// 突進ステート更新
	void UpdateLungesState(float elapsedTime);

	// 攻撃ステートへ遷移
	void TransitionAttackState();

	// 攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	// ダメージステートへ遷移
	void TransitionDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTimae);

private:

	enum class State
	{
		Move,
		Lunges,
		Attack,
		Damage,
		Death,
	};

	// アニメーション
	enum Animation
	{
		Anim_Run,
		Anim_Stop,
		Anim_Damage,
	};

private:
	std::unique_ptr<Model>  model[3];
	std::unique_ptr<Sprite> ui[6];
	std::unique_ptr<Sprite> hpSprite[4];
	std::unique_ptr<Text>   text[4];
	std::unique_ptr<Sprite> runAttack[3];
	std::unique_ptr<Sprite> itemImage[3];
	std::unique_ptr<Text>   itemNum[3];

	DirectX::XMFLOAT2 hpSpriteMinus = { 26, -40 };

	float viewMoveSpeedPlusTimer = 0.0f; // スピードを増やす処理
	float viewMoveSpeed = 0.0f;

	DirectX::XMFLOAT3 viewMoveSpeedColor = { 1, 1, 1 }; // 表示スピードの色
	DirectX::XMFLOAT3 viewMoveSpeedColorSpeed = { 0.8f, 1.3f, 1.6f }; // 色を変えるスピード

	struct ViewMoveSpeedColorUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}viewMoveSpeedColorUp;

	float turnSpeed = DirectX::XMConvertToRadians(80);

	float minAngleX = DirectX::XMConvertToRadians(-45); // 角度最小値
	float maxAngleX = DirectX::XMConvertToRadians(45);  // 角度最大値

	bool  lunges            = false; // 突進するか
	float lungesChargeTimer = 0.0f;  // 突進チャージ時間
	bool lungesSound = false;

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;
	YazirusiManager yazirusiManager;

	std::unique_ptr<Effect> hitEffect;
	std::unique_ptr<Effect> sppedEffect;
	Effekseer::Handle speedHandle;
	std::unique_ptr<Effect> invincibleEffect;
	Effekseer::Handle invincibleHandle;
	bool invincibleEffectPlay = false;
	bool invincibleState = false;;

	State state = State::Move;

	float playerAnimeCount = 0.0f;
	float leftHandRadius = 0.4f;
	bool attackCollisionFlag = false;

/****************************************************/

	int diffusionAttacks = 0;

	int exp    = 0;
	int expMax = 10;
	int level  = 1;

	float moveSpeed = 15.0f;

	int bananaNum = 0;

	int lungesCount = 0;
};