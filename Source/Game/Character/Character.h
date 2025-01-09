#pragma once

#include<DirectXMath.h>
#include <memory>

// キャラクター
class Character : public std::enable_shared_from_this<Character>
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
	bool ApplyDamage(int damage, float invincibleTime, float defensesUpTime);

	/**************************************************************************************/
		/*! セッター */
#if 1

	// HP設定
	void SetHp(int hp)
	{
		this->hp = hp;
		this->hpDamage = hp;
	}

	// HPの形態変化の設定
	void SetHpPhaseChange(float hpPhaseChangeTimer)
	{
		if (!hpPhaseChange)
		{
			hpPhaseChange = true;               // HPの形態変化の演出をする
			this->hpPhaseChangeTimer = hpPhaseChangeTimer; // HPの形態変化の演出をする時間
		}
	}

	//---------------------------------------------------------------------------//

		// マテリアルの色の設定
	void SetMaterialColor(const DirectX::XMFLOAT3 materialColor) { this->materialColor = materialColor; }

	// マテリアルの色変更の設定
	void SetMaterialColorChange(DirectX::XMFLOAT3 toMaterialColorChange, DirectX::XMFLOAT3 materialColorChangeSpeed)
	{
		if (!this->materialColorChange)
		{
			this->materialColorChange = true;                     // マテリアルの色を変える
			this->toMaterialColorChange = toMaterialColorChange;    // ここまでマテリアルの色を変える
			this->materialColorChangeSpeed = materialColorChangeSpeed; // マテリアルの色を変える速さ

			// マテリアルカラーXが目指す値より小さい時
			if (materialColor.x < toMaterialColorChange.x)
				materialColorChangeUp.x = true;
			else
				materialColorChangeUp.x = false;

			// マテリアルカラーYが目指す値より小さい時
			if (materialColor.y < toMaterialColorChange.y)
				materialColorChangeUp.y = true;
			else
				materialColorChangeUp.y = false;

			// マテリアルカラーZが目指す値より小さい時
			if (materialColor.z < toMaterialColorChange.z)
				materialColorChangeUp.z = true;
			else
				materialColorChangeUp.z = false;
		}
	}

	// 不透明度を設定
	void SetOpacity(float opacity) { this->opacity = opacity; }

	// 不透明度変更処理の設定
	void SetOpacityChange(float toOpacityChange, float opacityChangeSpeed)
	{
		if (!this->opacityChange)
		{
			this->opacityChange = true;               // 不透明度を変える
			this->toOpacityChange = toOpacityChange;    // ここまで不透明度を変える
			this->opacityChangeSpeed = opacityChangeSpeed; // 不透明度を変える速さ

			// 不透明度が目指す値より小さい時
			if (opacity < toOpacityChange)
				opacityChangeUp = true;
			else
				opacityChangeUp = false;
		}
	}

	//---------------------------------------------------------------------------//

		// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// 位置変更処理の解除(バグ防止用)
	void SetPositionChangeUnlock() { this->positionChange = false; }

	// 位置変更の設定
	void SetPositionChange(const DirectX::XMFLOAT3& toPositionChange,
		const DirectX::XMFLOAT3& positionChangeSpeed, bool positionChangeSpeedContinuingToIncrease = false)
	{
		if (!positionChange)
		{
			this->positionChange = true;                // 位置を変える
			this->toPositionChange = toPositionChange;    // ここまで位置を変える
			this->positionChangeSpeed = positionChangeSpeed; // 位置を変える速さ
			// 速度が増え続けるか
			this->positionChangeSpeedContinuingToIncrease = positionChangeSpeedContinuingToIncrease;
			// 増える速度
			if (positionChangeSpeedContinuingToIncrease)
				this->positionChangeSpeedIncrease = positionChangeSpeed;

			/// 位置Xが目指す位置より小さい時
			if (position.x < toPositionChange.x)
				positionChangeUp.x = true;
			else
				positionChangeUp.x = false;

			// 位置Yが目指す位置より小さい時
			if (position.y < toPositionChange.y)
				positionChangeUp.y = true;
			else
				positionChangeUp.y = false;

			// 位置Zが目指す位置より小さい時
			if (position.z < toPositionChange.z)
				positionChangeUp.z = true;
			else
				positionChangeUp.z = false;
		}
	}

	// 重力設定
	void SetGravity(float gravity) { this->gravity = gravity; }

	// 加速度設定
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	// 当たり判定の位置調整の設定
	void SetCollisionOffset(const DirectX::XMFLOAT3& collisionOffset) { this->collisionOffset = collisionOffset; }

	//---------------------------------------------------------------------------//

		// 角度設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// 角度変更の設定
	void SetAngleChange(const DirectX::XMFLOAT3& toAngleChange,
		const DirectX::XMFLOAT3& angleChangeSpeed)
	{
		if (!this->angleChange)
		{
			this->angleChange = true;             // 角度を変える
			this->toAngleChange = toAngleChange;    // ここまで角度を変える
			this->angleChangeSpeed = angleChangeSpeed; // 角度を変える速さ

			/// 角度Xが目指す大きさより小さい時
			if (angle.x < toAngleChange.x)
				angleChangeUp.x = true;
			else
				angleChangeUp.x = false;

			// 角度Yが目指す大きさより小さい時
			if (angle.y < toAngleChange.y)
				angleChangeUp.y = true;
			else
				angleChangeUp.y = false;

			// 角度Zが目指す大きさより小さい時
			if (angle.z < toAngleChange.z)
				angleChangeUp.z = true;
			else
				angleChangeUp.z = false;
		}
	}

	// 角度の回転の設定(回転速度の向き(±)は手動)
	void SetAngleRotation(float rotationTime, const DirectX::XMFLOAT3& angleRotationSpeed)
	{
		if (!this->angleRotation)
		{
			this->angleRotation = true;               // 角度を回転させる
			this->rotationTime = rotationTime;       // 回転時間
			this->angleRotationSpeed = angleRotationSpeed; // 角度の回転速度
		}
	}

	//---------------------------------------------------------------------------//

		// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// スケール変更の設定
	void SetScaleChange(const DirectX::XMFLOAT3& toScaleChange,
		const DirectX::XMFLOAT3& scaleChangeSpeed)
	{
		if (!this->scaleChange)
		{
			this->scaleChange = true;             // スケールを変える
			this->toScaleChange = toScaleChange;    // ここまでスケールを変える
			this->scaleChangeSpeed = scaleChangeSpeed; // スケールを変える速さ

			/// スケールXが目指す大きさより小さい時
			if (scale.x < toScaleChange.x)
				scaleChangeUp.x = true;
			else
				scaleChangeUp.x = false;

			// スケールYが目指す大きさより小さい時
			if (scale.y < toScaleChange.y)
				scaleChangeUp.y = true;
			else
				scaleChangeUp.y = false;

			// スケールZが目指す大きさより小さい時
			if (scale.z < toScaleChange.z)
				scaleChangeUp.z = true;
			else
				scaleChangeUp.z = false;
		}
	}

	//---------------------------------------------------------------------------//

		// ムービー中のアニメーション設定
	void SetMovieAnimation(int movieAnimNum, bool movieAnimLoop)
	{
		this->movieAnimation = false;         // アニメーションを設定できるようにする
		this->movieAnimNum = movieAnimNum;  // アニメーションの番号
		this->movieAnimLoop = movieAnimLoop; // アニメーションをループさせるか
	}

	// ムービー時間設定
	void SetMovieTime(float movieTime)
	{
		if (!movieScene)
		{
			movieScene = true;
			this->movieTime = movieTime;
		}
	}

	// アクションタイマーを設定
	void SetActionTimer(float actionTimer) { this->actionTimer = actionTimer; }

	// アクションを終了するまでの待ち時間を設定
	void SetActionFinishWaitTimer(float actionFinishWaitTimer) { this->actionFinishWaitTimer = actionFinishWaitTimer; }

	// 無敵時間設定
	void SetInvincibleTimer(float invincibleTimer) { this->invincibleTimer = invincibleTimer; }

	//---------------------------------------------------------------------------//

		// スプライト非表示設定
	void SetHideSprites(bool hideSprites) { this->hideSprites = hideSprites; }

	//---------------------------------------------------------------------------//

		// 半径設定
	void SetRadius(float radius) { this->radius = radius; }

	// 高さ設定
	void SetHeight(float height) { this->height = height; }

	//---------------------------------------------------------------------------//
		/*! カメラ */
#if 1
	// カメラのターゲットの位置変更の設定
	void SetCameraTargetChange(DirectX::XMFLOAT3 toCameraTarget, DirectX::XMFLOAT3 cameraTargetChangeSpeed);

	// カメラの角度変更の設定
	void SetCameraAngleChange(DirectX::XMFLOAT3 toCameraAngle, DirectX::XMFLOAT3 cameraAngleChangeSpeed);

	// カメラの範囲の変更の設定
	void SetCameraRangeChange(float toCameraRange, float cameraRangeChangeSpeed);

	// カメラシェイク設定
	void SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower);
#endif

	//---------------------------------------------------------------------------//
		/*! シェーダー */

		//! ポストエフェクト
#if 1
	// ポストエフェクトのステータスを設定
	void SetPostEffectStatus(float contrast = 1.3f,
		float saturation = 0.7f,
		const DirectX::XMFLOAT3 ColorFilter = { 0.9f, 1.0f, 1.05f },
		float chromaticAberration = 0.01f);

	// ポストエフェクトの変更の設定
	void SetPostEffectStatusChange(float toContrastChange = 1.3f,
		float contrastChangeSpeed = 1.0f,
		float toSaturationChange = 0.7f,
		float saturationChangeSpeed = 1.0f,
		const DirectX::XMFLOAT3 toColorFilterChange = { 0.9f, 1.0f, 1.05f },
		const DirectX::XMFLOAT3 colorFilterChangeSpeed = { 1.0f, 1.0f, 1.0f },
		float toChromaticAberrationChange = 0.1f,
		float chromaticAberrationChangeSpeed = 1.0f
	);

	// カラーフィルターを上昇させるかの設定
	void IncreaseColorFilterComponent(float colorFilter, float toColorFilterChange, bool& colorFilterUp);
#endif

	// エミッシブの色の設定
	void SetEmissiveColor(const DirectX::XMFLOAT3 emissiveColor) { this->emissiveColor = emissiveColor; }

	// エミッシブの色変更の設定
	void SetEmissiveColorChange(DirectX::XMFLOAT3 toEmissiveColorChange, DirectX::XMFLOAT3 emissiveColorChangeSpeed)
	{
		if (!this->emissiveColorChange)
		{
			this->emissiveColorChange = true;                     //エミッシブの色を変える
			this->toEmissiveColorChange = toEmissiveColorChange;    // ここまでエミシッブの色を変える
			this->emissiveColorChangeSpeed = emissiveColorChangeSpeed; //エミッシブの色を変える速さ

			//エミッシブカラーXが目指す値より小さい時
			if (emissiveColor.x < toEmissiveColorChange.x)
				emissiveColorChangeUp.x = true;
			else
				emissiveColorChangeUp.x = false;

			//エミッシブカラーYが目指す値より小さい時
			if (emissiveColor.y < toEmissiveColorChange.y)
				emissiveColorChangeUp.y = true;
			else
				emissiveColorChangeUp.y = false;

			//エミッシブカラーZが目指す値より小さい時
			if (emissiveColor.z < toEmissiveColorChange.z)
				emissiveColorChangeUp.z = true;
			else
				emissiveColorChangeUp.z = false;
		}
	}

	// エミッシブの強さの設定
	void SetEmissiveStrength(float emissiveStrength) { this->emissiveStrength = emissiveStrength; }

	// エミッシブの強さ変更の設定(設定の時に色を変えれる)
	void SetEmissiveStrengthChange(DirectX::XMFLOAT3 emissiveColor, float toEmissiveStrengthChange, float emissiveStrengthChangeSpeed)
	{
		if (!this->emissiveStrengthChange)
		{
			this->emissiveColor = emissiveColor;               //エミッシブの色
			this->emissiveStrengthChange = true;                        //エミッシブの強さを変える
			this->toEmissiveStrengthChange = toEmissiveStrengthChange;    // ここまでエミッシブの強さを変える
			this->emissiveStrengthChangeSpeed = emissiveStrengthChangeSpeed; // エミッシブの強さを変える速さ

			//!エミッシブの強さが目指す値より小さい時
			if (emissiveStrength < toEmissiveStrengthChange)
				emissiveStrengthChangeUp = true;
			else
				emissiveStrengthChangeUp = false;
		}
	}

	// エミッシブの形態変化演出の設定
	void SetEmissivePhaseChange(float emissiveTimerPhaseChangeTimer, float emissiveStrength)
	{
		if (!emissivePhaseChange)
		{
			emissivePhaseChange = true;                          //エミッシブの形態変化演出をする
			this->emissivePhaseChangeTimer = emissiveTimerPhaseChangeTimer; //エミッシブの形態変化演出をする時間
			this->emissiveStrength = emissiveStrength;              //エミッシブの強さ
		}
	}

#endif

	/**************************************************************************************/
		/*! ゲッター */
#if 1

	// HPを取得
	int GetHp() const { return hp; }

	// HPダメージ取得
	int GetHpDamage() const { return hpDamage; }

	// 最大HPを取得
	int GetMaxHp() const { return maxHp; }

	//---------------------------------------------------------------------------//

		// ステート変更までの時間を取得
	float GetStateChangeWaitTimer()  const { return stateChangeWaitTimer; }

	// アクションタイマーを取得
	float GetActionTimer() const { return actionTimer; }

	// アクション終了までの待ち時間を取得
	float GetActionFinishWaitTimer() const { return actionFinishWaitTimer; }

	// ヒットストップ時間の取得
	float GetHitStopElapsedTime() { return hitStopElapsedTime; }

	// 無敵時間取得
	float GetInvincibleTimer() const { return invincibleTimer; }

	// 防御力アップ時間取得
	float GetDefensesUpTimer() const { return defensesUpTimer; }

	// HP処理終了取得
	bool GetHpDirectorFinished() const { return hpDirectorFinished; }

	//---------------------------------------------------------------------------//

		// ムービー中かどうかを取得
	bool GetMovieScene() const { return movieScene; }

	// ムービー時間取得
	float GetMovieTime() const { return movieTime; }

	//---------------------------------------------------------------------------//

		// 不透明度を変えているかを取得
	bool GetOpacityChange() { return opacityChange; }

	// エミッシブの色を変えているかを取得
	bool GetEmissiveColorChange() { return emissiveColorChange; }

	// エミッシブの強さを変えているかを取得
	bool GetEmissiveStrengthChange() { return emissiveStrengthChange; }

	//---------------------------------------------------------------------------//

		// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// 位置変更をしているかを取得
	bool GetPositionChange() { return positionChange; }

	// 当たり判定の位置取得
	const DirectX::XMFLOAT3& GetCollisionPos() const { return collisionPos; }

	// 当たり判定の調整を取得
	const DirectX::XMFLOAT3& GetCollisionOffset() const { return collisionOffset; }

	// 加速度取得
	const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }

	//---------------------------------------------------------------------------//

		// 角度取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	// 角度変更をしているかを取得
	bool GetAngleChange() { return angleChange; }

	//---------------------------------------------------------------------------//

		// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	// スケール変更をしているかを取得
	bool GetScaleChange() { return scaleChange; }

	//---------------------------------------------------------------------------//

		// 半径取得
	float GetRadius() { return radius; }

	// 高さ取得
	float GetHeight() { return height; }

#endif

protected:

	/*! HP関連 */
#if 1

	// HP管理
	void HpControll(float elapsedTime);

	// HP演出
	bool HpDirector(int hpPlusNum = 0, int doNum = 0);

	// HPピンチ更新処理(敵のみ)
	bool UpdateHpPinchEnemy();

	/*! 状態更新処理 */

	// HPシェイク
	bool UpdateHpShake(float elapsedTime);

	// HPの形態変化演出
	bool UpdateHpPhaseChange(float elapsedTime);

	// HP画像の色変更更新処理
	void UpdateHpColorChange(float& hpImageColor, float& hpImageColorChangeSpeed, bool& hpImageColorUp, float elapsedTime);

	// HPゲージの色変更更新処理
	void UpdateHpSpriteChange(float& hpSpriteColor, float& hpSpriteColorChangeSpeed, bool& hpSpriteColorDown, float elapsedTime);

#endif

	/**************************************************************************************/
		/*! 速力処理 */
#if 1
	// 速力処理更新
	void UpdateVelocity(float elapsedTime);

	// 垂直速力更新処理
	void UpdateVerticalVelocity(float elapsedFrame);

	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedFrame);

	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

#endif

	/**************************************************************************************/

		/*! 移動関連 */
#if 1

	// スティック入力値から移動ベクトルを習得(3D)
	DirectX::XMFLOAT3 GetMoveVec() const;

	// スティック入力値から移動ベクトルを習得(2D)
	DirectX::XMFLOAT3 GetMoveVec_2D() const;

	// 移動処理(3D)
	void Move3D(float vx, float vz, float speed);

	// 移動処理(2D)
	void Move2D(float vx, float speed);

	// 旋回処理(ゼロベクトルでも処理を止めない)
	void Turn3DNotCancel(float elapsedTime, float vx, float vz, float speed);

	// 旋回処理(3D)
	void Turn3D(float elapsedTime, float vx, float vz, float speed);

	// 旋回処理(2D)
	void Turn2D(float elapsedTime, float vx, float speed);

	// ジャンプ処理
	void Jump(float speed);

#endif

	/**************************************************************************************/

		// 状態更新処理
	void UpdateCharacterState(float elapsedTime);

	// 状態更新処理関連(上の関数にまとめられている)
#if 1

	// マテリアルの色変更更新処理
	bool UpdateMaterialColorChange(float elapsedTime);

	// 単一軸のマテリアルの色変更更新処理
	float UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime);

	// 不透明度変更更新処理
	bool UpdateOpacityChange(float elapsedTime);

	// エミッシブの色変更更新処理
	bool UpdateEmissiveColorChange(float elapsedTime);

	// 単一軸のエミシッブの色変更更新処理
	float UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime);

	//エミッシブの強さ変更更新処理
	bool UpdateEmissiveStrengthChange(float elapsedTime);

	//エミッシブの形態変化演出更新処理
	bool UpdateEmissivePhaseChange(float elapsedTime);

	//エミッシブの色変更更新処理(形態変化)
	void UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeSpeed, bool& emissiveColorUp, float elapsedTime);

	//---------------------------------------------------------------------------//

		// 位置変更更新処理
	bool UpdatePositionChange(float elapsedTime);

	// 単一軸の位置変更更新処理
	float UpdatePositionAxis(float position, float& speed, float positionChangeSpeedIncrease, bool positionChangeUp, float toPositionChange, float elapsedTime);

	//---------------------------------------------------------------------------//

		// 角度制限処理
	void LimitAngle();

	// 回転変更更新処理
	bool UpdateAngleChange(float elapsedTime);

	// 単一軸の角度変更更新処理
	float UpdateAngleAxis(float angle, float speed, bool angleChangeUp, float toAngleChange, float elapsedTime);

	// 角度の回転の更新処理(特定の値は目指さず、指定された時間中に回転する)
	bool UpdateAngleRotation(float elapsedTime);

	//---------------------------------------------------------------------------//

		// スケール変更更新処理
	bool UpdateScaleChange(float elapsedTime);

	// 単一軸のスケール変更更新処理
	float UpdateScaleAxis(float scale, float speed, bool scaleChangeUp, float toScaleChange, float elapsedTime);

	//---------------------------------------------------------------------------//

		// ムービー時間更新処理
	bool UpdateMovieTimer(float elapsedTime);

	// ヒットストップ更新処理
	bool UpdateHitStop(float elapsedTime);

	// 無敵時間更新
	void UpdateInvincibleTimer(float elapsedTime);

	// 防御力アップ時間更新
	void UpdateDefensesUpTimer(float elapsedTime);

	//---------------------------------------------------------------------------//

#endif

	// 着地した時に呼ばれる
	virtual void OnLanding() {}

	// ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {};

	// 死亡した時に呼ばれる
	virtual void OnDead();

protected:
	DirectX::XMFLOAT3    collisionPos = { 0, 0, 0 };
	DirectX::XMFLOAT3    collisionOffset = { 0, 0, 0 };
	DirectX::XMFLOAT4X4  transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	// 円柱などの当たり判定の大きさ
	float radius = 0.45f; // 半径
	float height = 1.4f;  // 高さ

	float gravity = 0.5f; // 重力

	bool isGround = false; // 地面にいる

	/*****************************************************************************************************/
		/*! マテリアル */
	DirectX::XMFLOAT3 materialColor = { 1,1,1 }; // マテリアルの色
	bool materialColorChange = false;            // マテリアルの色を変更するか
	//マテリアルの色を増やすか
	struct MaterialColorChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}materialColorChangeUp;  // マテリアルの色を増やすか

	DirectX::XMFLOAT3 toMaterialColorChange = { 0, 0, 0 }; // ここまでマテリアルの色を変える
	DirectX::XMFLOAT3 materialColorChangeSpeed = { 0, 0, 0 }; // マテリアルの色を変える速さ

	//! 不透明度
	float opacity = 1.0f;  // 不透明度
	bool  opacityChange = false; // 不透明度を変更するか
	bool  opacityChangeUp = false; // 不透明度を増やすか
	float toOpacityChange = 0.0f;  // ここまで不透明度を変える 
	float opacityChangeSpeed = 0.0f;  // 不透明度を変える速度

	/*****************************************************************************************************/
		/*!エミッシブ */
	DirectX::XMFLOAT3 emissiveColor = { 0.0f, 0.0f, 0.0f }; //エミッシブカラー
	bool emissiveColorChange = false; //エミッシブの色を変えるか
	//エミッシブの色を増やすか
	struct EmissiveColorChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}emissiveColorChangeUp;  //エミッシブの色を増やすか

	DirectX::XMFLOAT3 toEmissiveColorChange = { 0, 0, 0 }; // ここまでエミシッブの色を変える
	DirectX::XMFLOAT3 emissiveColorChangeSpeed = { 0, 0, 0 }; // エミッシブの色を変える速さ

	//!エミッシブの強さ
	float emissiveStrength = 0.0f;  //エミッシブの強さ
	bool  emissiveStrengthChange = false; //エミッシブの強さを変えるか
	bool  emissiveStrengthChangeUp = false; //エミッシブの強さを増やすか

	float toEmissiveStrengthChange = 0.0f;  //　ここまでエミシッブの強さを変える
	float emissiveStrengthChangeSpeed = 0.0f;  //　エミシッブの強さを変える速さ

	//-------------------------------------------------------------------------------------------------------//

		/*! 形態変化演出(エミシッブ) */
	bool emissivePhaseChange = false; //エミッシブの形態変化演出をするか
	float emissivePhaseChangeTimer = 0.0f;  //エミッシブの形態変化演出をする時間
	float emissiveColorMin = 0.2f;  //エミッシブの色(最小値)
	float emissiveColorMax = 1.0f;  //エミッシブの色(最大値)
	DirectX::XMFLOAT3
		emissiveColorPhaseChangeSpeed = { 4.0f, 3.8f, 4.4f }; //エミッシブの色を変える速度(形態変化演出)
	struct EmissiveColorChangeDown
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}emissiveColorChangeDown;   //エミッシブの色を減らすか(上の物とは使用用途が違う)

	/*****************************************************************************************************/
		/*! 位置 */
#if 1

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	bool positionChange = false; // 位置を変更するか
	bool positionChangeSpeedContinuingToIncrease = false; // 速度が増え続けるか
	// 位置を増加させるか
	struct PositionChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}positionChangeUp;

	DirectX::XMFLOAT3 toPositionChange = { 0, 0, 0 }; // ここまで位置を変える
	DirectX::XMFLOAT3 positionChangeSpeed = { 0, 0, 0 }; // 位置を変える速さ
	DirectX::XMFLOAT3 positionChangeSpeedIncrease = { 0, 0, 0 }; // 位置を変える速さの増える値
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 }; // 加速度

#endif

	/*****************************************************************************************************/
		/*! 角度 */
#if 1

	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	bool angleChange = false; // 角度を変更するか
	// 角度を増やすか
	struct AngleChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}angleChangeUp;

	DirectX::XMFLOAT3 toAngleChange = { 0, 0, 0 }; // ここまで角度を変える
	DirectX::XMFLOAT3 angleChangeSpeed = { 0, 0, 0 }; // 角度を変える速さ

	//! 回転(特定の値は目指さない)
	float angleRotation = false; // 角度を回転させるか
	float rotationTime = 0.0f;  // 回転時間
	DirectX::XMFLOAT3 angleRotationSpeed = { 0, 0, 0 }; // 角度の回転速度

#endif

	/*****************************************************************************************************/
		/*! スケール */
#if 1 

	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	bool scaleChange = false;// スケールを変更するか
	// スケールを大きくするか
	struct ScaleChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}scaleChangeUp;

	DirectX::XMFLOAT3 toScaleChange = { 0, 0, 0 }; // ここまでスケールを変える
	DirectX::XMFLOAT3 scaleChangeSpeed = { 0, 0, 0 }; // スケールを変える速さ

#endif

	/*****************************************************************************************************/
		/*! ムービー関連 */

	bool  movieScene = false;  // ムービー中かどうか
	float movieTime = 0.0f;   // ムービー時間
	bool  movieAnimation = false;  // ムービー中のアニメーションをしたか
	int   movieAnimNum = 0;      // ムービー中のアニメーション番号
	bool  movieAnimLoop = false;  // ムービー中アニメーションをにループさせるか

	/*****************************************************************************************************/
		/*! HP関連 */
#if 1

	int  hp = 10;    // HP
	int  maxHp = 5;    // 最大HP
	bool hpDirectorFinished = false; // HPの演出が終わったか
	int  doHpDirectorCount = 0;     // HPを増やした回数(HP演出用)

	//! HPダメージ関連(演出用)
	int   hpDamage = 5; // HPダメージ
	float hpDamageCount = 2; // ダメージ演出を行うまでの時間
	float hpDamageDirectorWaitCount = 0; // HPを減らす演出にディレイをかける
	float maxHpDamageDirectorWaitCount = 0; // 最大HP減らす処理待ち時間
	float deathWaitTimer = 0; // 消滅カウント

	float nameSpriteOpacity = 0.0f; // 名前の不透明度
	int hpSpriteHeight = 0;    // HP画像の高さ

#endif

	//-----------------------------------------------------------------------------------//
		//! HPの画像
	DirectX::XMFLOAT2 hpSpritePos = { 0.0f, 0.0f };  // HPゲージの位置
	DirectX::XMFLOAT3 hpSpriteColor = { 1, 0, 1 };     // HPゲージの色
	DirectX::XMFLOAT2 hpImagePos = { 0.0f, 0.0f };  // HP画像の位置
	DirectX::XMFLOAT3 hpImageColor = { 1, 1, 1 };     // HP画像の色

	DirectX::XMFLOAT3 hpSpriteColorP = { 0, 1, 0 }; // HPゲージの色(プレイヤー用)

	//-----------------------------------------------------------------------------------//
		//! HPシェイク
	float hpSpriteShakePosY = 0.0f; // HPゲージシェイクの位置
	float hpImageShakePosY = 0.0f; // HP画像シェイクの位置
	bool  hpShake = false;// HPシェイクするか
	float hpShakeTimer = 0.0f; // HPシェイクタイマー

	//-----------------------------------------------------------------------------------//
		//! HP形態変化
	bool  hpPhaseChange = false; // HPの形態変化演出
	float hpPhaseChangeTimer = 0.0f;  // HPの形態変化演出を行う時間

	//! HP画像の色変更
	float hpImageColorMin = 0.2f;                   // HP画像の色(最小値)
	float hpImageColorMax = 1.0f;                   // HP画像の色(最大値)
	DirectX::XMFLOAT3
		hpImageColorChangeSpeed = { 4.0f, 3.8f, 4.4f }; // HP画像の色を変える速度
	struct HpImageColorUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}hpImageColorUp;    // HP画像の色を増やすか

	//! HPゲージの色変更
	float hpSpriteColorMin = 0;     // HPゲージの色(最小値)
	float hpSpriteColorMax = 1.0f;  // HPゲージの色(最大値)
	float hpSpriteColorChangeSpeed = 3.8f;  // HPゲージの色を変える速度
	bool hpSpriteColorDown = false; // HPゲージの色を下げるか

	//-----------------------------------------------------------------------------------//
		//! ダメージ計算関連
	bool  isGuard = false; // ガードしているか
	int   damage = 0;     // ダメージ
	int   additionalDamage = 0;     // 追加ダメージ(防御力低下などを表現する)
	int   defenses = 0;     // 防御力
	float defensesUpTimer = 0.0f;    // 防御力アップ時間

	//-----------------------------------------------------------------------------------//
		//! その他
	bool  hideSprites = false; // HPなどを隠す
	float invincibleTimer = 0.0f;  // 無敵時間

	float stateChangeWaitTimer = 0.0f; // ステート切り替えまでの時間
	float actionTimer = 0.0f; // アクションタイマー
	float actionFinishWaitTimer = 0.0f; // アクション終了までの待ち時間

	/*****************************************************************************************************/
	/*! ヒットストップ */

	bool  hitStop = false; // ヒットストップ中か
	float hitStopElapsedTime = 1.0f;  // ヒットストップ込みのelapsedTime
	float hitStopSeconds = 0.0f;  // ヒットストップの時間
	float hitStopMaxSeconds = 0.3f;  // ヒットストップの最大の時間

	/*****************************************************************************************************/
	float friction = 0.5f; // 摩擦
	float acceleration = 3.0f; // 加速力
	float maxMoveSpeed = 5.0f; // 最大移動速度
	float moveVecX = 0.0f; // 移動ベクトルX
	float moveVecZ = 0.0f; // 移動ベクトルZ

	float stepOffset = 1.0f; // レイキャストのオフセット
};