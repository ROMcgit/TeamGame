#pragma once

#include<DirectXMath.h>

// キャラクター
class Character
{
public:
	Character() {}
	virtual ~Character() {}

	// 当たり判定の位置を設定
	void CollisionPosSettings();

	// 行列更新処理
	void UpdateTransform();

	// 地面に接地しているか
	bool IsGround() const { return isGround; }

	// ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime);

	// 衝撃を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

/**************************************************************************************/

	/*! セッター */

	// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// スケール変更の設定
	void SetScaleChange(const DirectX::XMFLOAT3& toScaleChange, const DirectX::XMFLOAT3& scaleChangePower)
	{
		this->scaleChange = true;                // スケールを変える
		this->toScaleChange = toScaleChange;       // ここまでスケールを変える
		this->scaleChangePower = scaleChangePower; // 1秒間にスケールを変える大きさ

		/// Xスケールが目指す大きさより小さい時
		if (scale.x < toScaleChange.x)
			scaleChageUp.x = true;
		else
			scaleChageUp.x = false;

		// Yスケールが目指す大きさより小さい時
		if (scale.y < toScaleChange.y)
			scaleChageUp.y = true;
		else
			scaleChageUp.y = false;

		// Zスケールが目指す大きさより小さい時
		if (scale.z < toScaleChange.z)
			scaleChageUp.z = true;
		else
			scaleChageUp.z = false;
	}

	// 加速度設定
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	// スプライト非表示設定
	void SetHideSprites(bool hideSprites) { this->hideSprites = hideSprites; }

	// ムービー時間設定
	void SetMovieTime(float movieTime)
	{
		movieScene = true;
		this->movieTime = movieTime;
	}

/*******************************************************/

	/*! ゲッター */

	// 位置情報
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// 当たり判定の位置取得
	const DirectX::XMFLOAT3& GetCollisionPos() const { return collisionPos; }

	// 回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	// 半径取得
	float GetRadius() const { return radius; }

	// 高さ取得
	float GetHeight() const { return height; }

	// 健康状態を取得
	int GetHealth() const { return hp; }

	// 最大健康状態を取得
	int GetMaxHealth() const { return maxHp; }

	// 無敵時間取得
	float GetInvincibleTimer() const { return invincibleTimer; }

protected:
	// スティック入力値から移動ベクトルを習得
	DirectX::XMFLOAT3 GetMoveVec() const;

	// 移動処理
	void Move(float vx, float vz, float speed);

	// 旋回処理
	void Turn(float elapsedTime, float vx, float vz, float speed);

	// ジャンプ処理
	void Jump(float speed);

	// 速力処理更新
	void UpdateVelocity(float elapsedTime);

	// HP管理
	void HpControll(float elapsedTime);

	// HP演出
	bool HpDirector(int hpPlusNum = 0, int doNum = 0);

	// スケール変更更新処理
	bool UpdateScaleChange(float elapsedTime);

	// HPシェイク
	bool UpdateHpShake(float elapsedTime);

	// 無敵時間更新
	void UpdateInvincibleTimer(float elapsedTime);

	// ムービーシーンの時間処理
	bool UpdateMovieTimer(float elapsedTime);

	// 垂直速力更新処理
	void UpdateVerticalVelocity(float elapsedFrame);

	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedFrame);

	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

	// 着地した時に呼ばれる
	virtual void OnLanding() {}

	// ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {};

	// 死亡した時に呼ばれる
	virtual void OnDead();

protected:
	DirectX::XMFLOAT3    position     = { 0, 0, 0, };
	DirectX::XMFLOAT3    collisionPos = { 0, 0, 0 };
	DirectX::XMFLOAT3    collisionOffset = { 0, 0, 0 };
	DirectX::XMFLOAT3    angle        = { 0, 0, 0 };
	DirectX::XMFLOAT3    scale        = { 1,1,1 };
	DirectX::XMFLOAT4X4  transform    = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float radius = 0.5f;
	float height = 2.0f;

	float			  gravity = -0.3f;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0, };

	bool isGround = false;

	bool scaleChange = false;// スケールを変更するか
	// スケールを大きくするか
	struct ScaleChageUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}scaleChageUp;

	DirectX::XMFLOAT3 toScaleChange = { 0, 0, 0 }; // ここまで大きさを変える
	DirectX::XMFLOAT3 scaleChangePower = { 0, 0, 0 }; // スケールを変える大きさ

	bool movieScene     = false; // ムービー中かどうか
	float movieTime     = 0.0f;  // ムービー時間
	bool movieAnimation = false; // ムービー中のアニメーションをしたか
	int movieAnimNum    = 0;     // ムービー中のアニメーション番号
	bool movieAnimLoop  = false; // ムービー中アニメーションをにループさせるか

/********************************************************************************/

	int hp                     = 10;     // HP
	int maxHp                  = 5;      // 最大HP
	int hpDamage               = 5;      // HPダメージ
	bool hpDirectorFinished    = false;  // HPの演出が終わったか
	int doHpDirectorCount      = 0;      // HPを増やした回数(HP演出用)
	float doHpDirectorWaitTime = 0.005f; // HP増やすまでの待ち時間

	float nameSpriteOpacity = 0.0f;       // 名前の不透明度
	int hpSpriteHeight      = 0;          // HP画像の高さ

	DirectX::XMFLOAT2 hpSpritePos = { 0.0f, 0.0f };  // HPゲージの位置
	float hpSpriteShakePosY = 0.0f;            // HPゲージシェイクの位置
	DirectX::XMFLOAT2 hpImagePos = { 0.0f, 0.0f };  // HP画像の位置
	DirectX::XMFLOAT3 hpSpriteColor = { 1, 1, 1 };     // HP画像の色

	float hpImageShakePosY = 0.0f;  // HP画像シェイクの位置
	float hpSpriteAdjust = 0.0f;  // HPの描画の調整
	bool  hpShake = false; // HPシェイクするか
	float hpShakeTimer = 0.0f;  // HPシェイクタイマー
	bool hideSprites = false; // HPなどを隠す

	float invincibleTimer = 0.0f; // 無敵時間

	float hpDamageCount = 2; // ダメージ演出を行うまでの時間
	float hpDamageDirectorWaitCount = 0; // HPを減らす演出にディレイをかける
	float maxHpDamageDirectorWaitCount = 0; // 最大HP減らす処理待ち時間
	float deathWaitTimer = 0;         // 消滅カウント

	int damage = 0;    // ダメージ
	int defenses = 0;    // 防御力
	float defensesUpTimer = 0.0f; // 防御力アップ時間

	float stateChangeWaitTimer = 0.0f;   // ステート切り替えまでの時間
	float actionTimer = 0.0f; // アクションタイマー

/********************************************************************************/

	float friction = 0.5f;

	float acceleration = 1.0f;
	float maxMoveSpeed = 12.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;

	float airControl = 0.3f;

	float stepOffset = 1.0f;

	float slopeRate = 1.0f;
};