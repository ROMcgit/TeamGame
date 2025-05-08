#pragma once

#include<DirectXMath.h>
#include <memory>
#include <stdexcept>
#include <algorithm>

#include "Graphics/Fade.h"
#include "Game/Effect/Effect.h"
#include "Graphics/Model.h"

class GameObjectBase
{
public:
	GameObjectBase() {}
	virtual ~GameObjectBase() {}

/**************************************************************************************/
	/*! どのイージングにするか */
#if 1

	//! 位置
	struct PositionChangeEasing
	{
		//! イージングタイプ
		enum class Type
		{
			Linear,          // リニア(補完無し)
			EaseIn,          // イーズイン(加速)
			EaseOut,         // イーズアウト(減速)
			EaseInOut,       // イーズインアウト(加速→減速)
			EaseBack,        // バック(始まりや終わりに戻りを加える)
			EaseBounce,      // バウンド(弾む動き)
			EaseElastic,     // エラスティック(弾性のような動き)
			EaseInCircle,    // イーズインサークル(円形加速)
			EaseOutCircle,   // イーズアウトサークル(円形減速)
			EaseInOutCircle, // イーズインアウトサークル(円形加速→円形減速)
		};

		//! イージング設定
		Type all = Type::Linear;
		Type x = Type::Linear;
		Type y = Type::Linear;
		Type z = Type::Linear;
	}positionChangeEasing;

	//! 角度
	struct AngleChangeEasing
	{
		enum class Type
		{
			Linear,          // リニア(補完無し)
			EaseIn,          // イーズイン(加速)
			EaseOut,         // イーズアウト(減速)
			EaseInOut,       // イーズインアウト(加速→減速)
			EaseBack,        // バック(始まりや終わりに戻りを加える)
			EaseBounce,      // バウンド(弾む動き)
			EaseElastic,     // エラスティック(弾性のような動き)
			EaseInCircle,    // イーズインサークル(円形加速)
			EaseOutCircle,   // イーズアウトサークル(円形減速)
			EaseInOutCircle, // イーズインアウトサークル(円形加速→円形減速)
		};

		//! イージング設定
		Type all = Type::Linear;
		Type x = Type::Linear;
		Type y = Type::Linear;
		Type z = Type::Linear;
	}angleChangeEasing;

	//! スケール
	struct ScaleChangeEasing
	{
		enum class Type
		{
			Linear,          // リニア(補完無し)
			EaseIn,          // イーズイン(加速)
			EaseOut,         // イーズアウト(減速)
			EaseInOut,       // イーズインアウト(加速→減速)
			EaseBack,        // バック(始まりや終わりに戻りを加える)
			EaseBounce,      // バウンド(弾む動き)
			EaseElastic,     // エラスティック(弾性のような動き)
			EaseInCircle,    // イーズインサークル(円形加速)
			EaseOutCircle,   // イーズアウトサークル(円形減速)
			EaseInOutCircle, // イーズインアウトサークル(円形加速→円形減速)
		};

		//! イージング設定
		Type all = Type::Linear;
		Type x = Type::Linear;
		Type y = Type::Linear;
		Type z = Type::Linear;

	}scaleChangeEasing;

#endif

	/*! どのイージングにするか(カメラ用) */
#if 1
	//! ターゲット
	enum class CameraTargetChangeEasing
	{
		Linear,  // リニア(補完無し)
		EaseIn,  // イーズイン(加速)
		EaseOut, // イーズアウト(減速)
	};

	static CameraTargetChangeEasing cameraTargetChangeEasing;

	//! 角度
	enum class CameraAngleChangeEasing
	{
		Linear,  // リニア(補完無し)
		EaseIn,  // イーズイン(加速)
		EaseOut, // イーズアウト(減速)
	};

	static CameraAngleChangeEasing cameraAngleChangeEasing;

	//! 範囲
	enum class CameraRangeChangeEasing
	{
		Linear,  // リニア(補完無し)
		EaseIn,  // イーズイン(加速)
		EaseOut, // イーズアウト(減速)
	};

	static CameraRangeChangeEasing cameraRangeChangeEasing;
#endif

/**************************************************************************************/

	//! どの当たり判定にするか
	enum class CollisionType
	{
		Box,      // ボックス
		Sphere,   // 球
		Cylinder, // 円柱
		Count,    // 要素数を取得するためのダミーエントリ
	}collisionType = CollisionType::Cylinder;

/**************************************************************************************/
	/*! セッター */
#if 1

	/*! その他 */
#if 1

	// 重力設定
	void SetGravity(float gravity) { this->gravity = gravity; }

	// 重力のリセットの設定
	void SetGravityReset() { this->gravity = gravityReset; }

	// 着地状態を解除(加速度を変更せずに位置変更処理で移動した用)
	void SetIsGroundUnlock() { this->isGround = false; }

	// スプライト非表示設定
	void SetHideSprites(bool hideSprites) { this->hideSprites = hideSprites; }

#endif

	/*! HP関連 */
#if 1

	// HP設定
	void SetHp(int hp)
	{
		this->hp = hp;
		this->hpDamage = hp;
	}

	// HPを増やす設定
	void SetHpPlus(int hpPlus) { this->hp += hpPlus; }

	// HPの形態変化の設定
	void SetHpPhaseChange(float hpPhaseChangeTimer)
	{
		if (!hpPhaseChange)
		{
			hpPhaseChange = true;               // HPの形態変化の演出をする
			this->hpPhaseChangeTimer = hpPhaseChangeTimer; // HPの形態変化の演出をする時間
		}
	}

#endif

//---------------------------------------------------------------------------//

	/*! 描画関連 */
#if 1

	// マテリアルの色の設定
	void SetMaterialColor(const DirectX::XMFLOAT3 materialColor) { this->materialColor = materialColor; }

	// マテリアルの色変更の設定
	void SetMaterialColorChange(DirectX::XMFLOAT3 endMaterialColorChange, float materialColorChangeTime)
	{
		if (!this->materialColorChange &&
			(materialColor.x != endMaterialColorChange.x
				|| materialColor.y != endMaterialColorChange.y
				|| materialColor.z != endMaterialColorChange.z))
		{
			this->materialColorChange = true;                    // マテリアルの色を変える
			this->startMaterialColorChange = this->materialColor;     // マテリアルの色の変更の開始の値
			this->endMaterialColorChange = endMaterialColorChange;  // ここまでマテリアルの色を変える
			this->materialColorChangeTime = materialColorChangeTime; // マテリアルの色を変える時間
			this->materialColorChangeElapsedTime = 0.0f;                    // 待ち時間をリセット

			/*! マテリアルの色の範囲を超えないようにする */
			//! マテリアルカラーX
			this->endMaterialColorChange.x = std::clamp(this->endMaterialColorChange.x, 0.0f, 1.0f);
			//! マテリアルカラーY
			this->endMaterialColorChange.y = std::clamp(this->endMaterialColorChange.y, 0.0f, 1.0f);
			//! マテリアルカラーZ
			this->endMaterialColorChange.z = std::clamp(this->endMaterialColorChange.z, 0.0f, 1.0f);

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->materialColorChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 不透明度を設定
	void SetOpacity(float radius) { this->radius = radius; }

	// 不透明度変更処理の設定
	void SetOpacityChange(float endOpacityChange, float opacityChangeTime)
	{
		if (!this->opacityChange && opacity != endOpacityChange)
		{
			this->opacityChange = true;              // 不透明度を変える
			this->startOpacityChange = this->opacity;     // 不透明度の変更の開始の値
			this->endOpacityChange = endOpacityChange;  // ここまで不透明度を変える
			this->opacityChangeTime = opacityChangeTime; // 不透明度を変える時間
			this->opacityChangeElapsedTime = 0.0f;              // 経過時間をリセット

			// 不透明度の範囲を超えないようにする
			this->endOpacityChange = std::clamp(this->endOpacityChange, 0.0f, 1.0f);

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->opacityChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 不透明度変更の解除
	void SetOpacityChangeUnlock() { this->opacityChange = false; }

//-----------------------------------------------------------------------------//

	// エミッシブの色の設定
	void SetEmissiveColor(const DirectX::XMFLOAT3 emissiveColor) { this->emissiveColor = emissiveColor; }

	// エミッシブの色変更の設定
	void SetEmissiveColorChange(DirectX::XMFLOAT3 endEmissiveColorChange, float emissiveColorChangeTime)
	{
		if (!this->emissiveColorChange &&
			(this->emissiveColor.x != endEmissiveColorChange.x
				|| this->emissiveColor.y != endEmissiveColorChange.y
				|| this->emissiveColor.z != endEmissiveColorChange.z))
		{
			this->emissiveColorChange = true;                    //エミッシブの色を変える
			this->startEmissiveColorChange = this->emissiveColor;     // エミッシブの強さの変更の開始の値
			this->endEmissiveColorChange = endEmissiveColorChange;  // ここまでエミシッブの色を変える
			this->emissiveColorChangeTime = emissiveColorChangeTime; //エミッシブの色を変える時間
			this->emissiveColorChangeElapsedTime = 0.0f;                    // 経過時間をリセット

			/*! エミッシブの色の範囲を超えないようにする */
			//! エミッシブカラーX
			this->endEmissiveColorChange.x = std::clamp(this->endEmissiveColorChange.x, 0.0f, 1.0f);
			//! エミッシブカラーY
			this->endEmissiveColorChange.y = std::clamp(this->endEmissiveColorChange.y, 0.0f, 1.0f);
			//! エミッシブカラーZ
			this->endEmissiveColorChange.z = std::clamp(this->endEmissiveColorChange.z, 0.0f, 1.0f);

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->emissiveColorChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// エミッシブの強さの設定
	void SetEmissiveStrength(float emissiveStrength) { this->emissiveStrength = emissiveStrength; }

	// エミッシブの強さ変更の設定(設定の時に色を変えれる)
	void SetEmissiveStrengthChange(DirectX::XMFLOAT3 emissiveColor, float endEmissiveStrengthChange, float emissiveStrengthChangeTime)
	{
		if (!this->emissiveStrengthChange && this->emissiveStrength != endEmissiveStrengthChange)
		{
			this->emissiveColor = emissiveColor;              //エミッシブの色
			this->emissiveStrengthChange = true;                       //エミッシブの強さを変える
			this->startEmissiveStrengthChange = this->emissiveStrength;     // エミッシブの強さの変更の開始の値
			this->endEmissiveStrengthChange = endEmissiveStrengthChange;  // ここまでエミッシブの強さを変える
			this->emissiveStrengthChangeTime = emissiveStrengthChangeTime; // エミッシブの強さを変える時間
			this->emissiveStrengthChangeElapsedTime = 0.0f;                       // 経過時間をリセット

			// エミッシブの強さの範囲を超えないようにする
			this->endEmissiveStrengthChange = std::clamp(this->endEmissiveStrengthChange, 0.0f, 1.0f);

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->emissiveStrengthChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// エミシッブの強さを0にする
	void SetEmissiveStrengthResetChange(float emissiveStrengthChangeTime = 0.1f)
	{
		//! 変更処理を解除
		this->emissiveStrengthChange = false;

		if (!this->emissiveStrengthChange)
		{
			this->emissiveStrengthChange = true;                       //エミッシブの強さを変える
			this->startEmissiveStrengthChange = this->emissiveStrength;     // エミッシブの強さの変更の開始の値
			this->endEmissiveStrengthChange = 0.0f;                       // ここまでエミッシブの強さを変える
			this->emissiveStrengthChangeTime = emissiveStrengthChangeTime; // エミッシブの強さを変える時間
			this->emissiveStrengthChangeElapsedTime = 0.0f;                 // 経過時間をリセット

			// エミッシブの強さの範囲を超えないようにする
			this->endEmissiveStrengthChange = std::clamp(this->endEmissiveStrengthChange, 0.0f, 1.0f);

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->emissiveStrengthChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// エミッシブの強さ変更の解除
	void SetEmissiveStrengthChangeUnlock() { emissiveStrengthChange = false; }

	// エミッシブの形態変化演出の設定
	void SetEmissivePhaseChange(float emissiveTimerPhaseChangeTimer, float emissiveStrength)
	{
		if (!emissivePhaseChange)
		{
			this->emissivePhaseChange = true;                          //エミッシブの形態変化演出をする
			this->emissivePhaseChangeTimer = emissiveTimerPhaseChangeTimer; //エミッシブの形態変化演出をする時間
			this->emissiveStrength = emissiveStrength;              //エミッシブの強さ
		}
	}

#endif

//---------------------------------------------------------------------------//

	//! 向きの設定
	void SetDirection(const DirectX::XMFLOAT3& direction);

	//! 移動速度設定
	void SetMoveSpeed(float moveSpeed) { this->moveSpeed = moveSpeed; }

	/*! 位置 */
#if 1
	// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// 位置X設定
	void SetPositionX(float positionX) { this->position.x = positionX; }

	// 位置Y設定
	void SetPositionY(float positionY) { this->position.y = positionY; }

	// 位置Z設定
	void SetPositionZ(float positionZ) { this->position.z = positionZ; }

//-------------------------------------------------------------------------//

	// 位置変更処理の解除(バグ防止用)
	void SetPositionChangeUnlock()
	{
		this->positionChange.all = false;
		this->positionChange.x = false;
		this->positionChange.y = false;
		this->positionChange.z = false;
	}

	// 位置全体変更の設定
	void SetPositionAllChange(const DirectX::XMFLOAT3& endPositionChange,
		float positionChangeTime, PositionChangeEasing::Type positionChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.all &&
			(position.x != endPositionChange.x
				|| position.y != endPositionChange.y
				|| position.z != endPositionChange.z))
		{
			this->positionChange.all = true;                 // 位置を変える
			this->startPositionChange = this->position;       // 位置の変更の開始の値
			this->endPositionChange = endPositionChange;    // ここまで位置を変える
			this->positionChangeTime.all = positionChangeTime;   // 位置を変える時間
			this->positionChangeEasing.all = positionChangeEasing; // そのイージングにするか
			this->positionChangeElapsedTime.all = 0.0f;                 // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->positionChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 位置X変更の設定
	void SetPositionXChange(float endPositionXChange,
		float positionXChangeTime, PositionChangeEasing::Type positionXChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.x && position.x != endPositionXChange)
		{
			this->positionChange.x = true;                  // 位置を変える
			this->startPositionChange.x = this->position.x;      // 位置の変更の開始の値
			this->endPositionChange.x = endPositionXChange;    // ここまで位置を変える
			this->positionChangeTime.x = positionXChangeTime;   // 位置を変える時間
			this->positionChangeEasing.x = positionXChangeEasing; // そのイージングにするか
			this->positionChangeElapsedTime.x = 0.0f;                  // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->positionChangeTime.x <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 位置Y変更の設定
	void SetPositionYChange(float endPositionYChange,
		float positionYChangeTime, PositionChangeEasing::Type positionYChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.y && position.y != endPositionYChange)
		{
			this->positionChange.y = true;                  // 位置を変える
			this->startPositionChange.y = this->position.y;      // 位置の変更の開始の値
			this->endPositionChange.y = endPositionYChange;    // ここまで位置を変える
			this->positionChangeTime.y = positionYChangeTime;   // 位置を変える時間
			this->positionChangeEasing.y = positionYChangeEasing; // そのイージングにするか
			this->positionChangeElapsedTime.y = 0.0f;                  // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->positionChangeTime.y <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 位置Z変更の設定
	void SetPositionZChange(float endPositionZChange,
		float positionZChangeTime, PositionChangeEasing::Type positionZChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.z && position.z != endPositionZChange)
		{
			this->positionChange.z = true;                  // 位置を変える
			this->startPositionChange.z = this->position.z;      // 位置の変更の開始の値
			this->endPositionChange.z = endPositionZChange;    // ここまで位置を変える
			this->positionChangeTime.z = positionZChangeTime;   // 位置を変える時間
			this->positionChangeEasing.z = positionZChangeEasing; // そのイージングにするか
			this->positionChangeElapsedTime.z = 0.0f;                  // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->positionChangeTime.z <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

#endif

	/*! 加速度 */
#if 1

	// 加速度を設定(全体)
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	// 加速度Xを設定
	void SetVelocityX(float velocityX) { this->velocity.x = velocityX; }

	// 加速度Yを設定
	void SetVelocityY(float velocityY) { this->velocity.y = velocityY; }

	// 加速度Zを設定
	void SetVelocityZ(float velocityZ) { this->velocity.z = velocityZ; }

#endif

//---------------------------------------------------------------------------//

	/*! 角度 */
#if 1

	// 角度設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// 角度X設定
	void SetAngleX(float angleX) { this->angle.x = angleX; }

	// 角度Y設定
	void SetAngleY(float angleY) { this->angle.y = angleY; }

	// 角度Z設定
	void SetAngleZ(float angleZ) { this->angle.z = angleZ; }

//-------------------------------------------------------------------------//

	// 角度変更の解除
	void SetAngleChangeUnlock()
	{
		this->angleChange.all = false;
		this->angleChange.x = false;
		this->angleChange.y = false;
		this->angleChange.z = false;
	}

	// 角度全体変更の設定
	void SetAngleAllChange(const DirectX::XMFLOAT3& endAngleChange,
		float angleChangeTime, AngleChangeEasing::Type angleChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!this->angleChange.all &&
			(angle.x != endAngleChange.x
				|| angle.y != endAngleChange.y
				|| angle.z != endAngleChange.z))
		{
			this->angleChange.all = true;              // 角度を変える
			this->startAngleChange = this->angle;       // 角度の変更の開始の値
			this->endAngleChange = endAngleChange;    // ここまで角度を変える
			this->angleChangeTime.all = angleChangeTime;   // 角度を変える時間
			this->angleChangeEasing.all = angleChangeEasing; // どのイージングにするか
			this->angleChangeElapsedTime.all = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->angleChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 角度X変更の設定
	void SetAngleXChange(float endAngleXChange,
		float angleXChangeTime, AngleChangeEasing::Type angleXChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!angleChange.x && angle.x != endAngleXChange)
		{
			this->angleChange.x = true;               // 角度Xを変える
			this->startAngleChange.x = this->angle.x;      // 角度Xの変更の開始の値
			this->endAngleChange.x = endAngleXChange;    // ここまで角度Xを変える
			this->angleChangeTime.x = angleXChangeTime;   // 角度Xを変える時間
			this->angleChangeEasing.x = angleXChangeEasing; // そのイージングにするか
			this->angleChangeElapsedTime.x = 0.0f;               // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->angleChangeTime.x <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 角度Y変更の設定
	void SetAngleYChange(float endAngleYChange,
		float angleYChangeTime, AngleChangeEasing::Type angleYChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!angleChange.y && angle.y != endAngleYChange)
		{
			this->angleChange.y = true;               // 角度Yを変える
			this->startAngleChange.y = this->angle.y;      // 角度Yの変更の開始の値
			this->endAngleChange.y = endAngleYChange;    // ここまで角度Yを変える
			this->angleChangeTime.y = angleYChangeTime;   // 角度Yを変える時間
			this->angleChangeEasing.y = angleYChangeEasing; // そのイージングにするか
			this->angleChangeElapsedTime.y = 0.0f;               // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->angleChangeTime.y <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 角度Z変更の設定
	void SetAngleZChange(float endAngleZChange,
		float angleZChangeTime, AngleChangeEasing::Type angleZChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!angleChange.z && angle.z != endAngleZChange)
		{
			this->angleChange.z = true;               // 角度Zを変える
			this->startAngleChange.z = this->angle.z;      // 角度Zの変更の開始の値
			this->endAngleChange.z = endAngleZChange;    // ここまで角度Zを変える
			this->angleChangeTime.z = angleZChangeTime;   // 角度Zを変える時間
			this->angleChangeEasing.z = angleZChangeEasing; // そのイージングにするか
			this->angleChangeElapsedTime.z = 0.0f;               // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->angleChangeTime.z <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 角度の回転の設定(回転速度の向き(±)は手動)
	void SetAngleRotation(const DirectX::XMFLOAT3& angleRotationSpeed, float rotationTime)
	{
		if (!this->angleRotation)
		{
			this->angleRotation = true;               // 角度を回転させる
			this->angleRotationSpeed = angleRotationSpeed; // 角度の回転速度
			this->rotationTime = rotationTime;       // 回転時間
		}
	}

	// 角度の回転の解除
	void SetAngleRotationUnlock() { this->angleRotation = false; }

#endif

//---------------------------------------------------------------------------//

	/*! スケール */
#if 1

	// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// スケール変更の解除
	void SetScaleChangeUnlock()
	{
		this->scaleChange.all = false;
		this->scaleChange.x = false;
		this->scaleChange.y = false;
		this->scaleChange.z = false;
	}

	// スケール全体変更の設定
	void SetScaleAllChange(const DirectX::XMFLOAT3& endScaleChange,
		float scaleChangeTime, ScaleChangeEasing::Type scaleChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!this->scaleChange.all &&
			(scale.x != endScaleChange.x
				|| scale.y != endScaleChange.y
				|| scale.z != endScaleChange.z))
		{
			this->scaleChange.all = true;              // スケールを変える
			this->startScaleChange = this->scale;       // スケールの変更の開始の値
			this->endScaleChange = endScaleChange;    // ここまでスケールを変える
			this->scaleChangeTime.all = scaleChangeTime;   // スケールを変える時間
			this->scaleChangeEasing.all = scaleChangeEasing; // どのイージングにするか
			this->scaleChangeElapsedTime.all = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->scaleChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// スケールX変更の設定
	void SetScaleXChange(float endScaleXChange,
		float scaleXChangeTime, ScaleChangeEasing::Type scaleXChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!scaleChange.x && scale.x != endScaleXChange)
		{
			this->scaleChange.x = true;               // スケールXを変える
			this->startScaleChange.x = this->scale.x;      // スケールXの変更の開始の値
			this->endScaleChange.x = endScaleXChange;    // ここまでスケールXを変える
			this->scaleChangeTime.x = scaleXChangeTime;   // スケールXを変える時間
			this->scaleChangeEasing.x = scaleXChangeEasing; // そのイージングにするか
			this->scaleChangeElapsedTime.x = 0.0f;               // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->scaleChangeTime.x <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// スケールY変更の設定
	void SetScaleYChange(float endScaleYChange,
		float scaleYChangeTime, ScaleChangeEasing::Type scaleYChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!scaleChange.y && scale.y != endScaleYChange)
		{
			this->scaleChange.y = true;               // スケールYを変える
			this->startScaleChange.y = this->scale.y;      // スケールYの変更の開始の値
			this->endScaleChange.y = endScaleYChange;    // ここまでスケールYを変える
			this->scaleChangeTime.y = scaleYChangeTime;   // スケールYを変える時間
			this->scaleChangeEasing.y = scaleYChangeEasing; // そのイージングにするか
			this->scaleChangeElapsedTime.y = 0.0f;               // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->scaleChangeTime.y <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// スケールZ変更の設定
	void SetScaleZChange(float endScaleZChange,
		float scaleZChangeTime, ScaleChangeEasing::Type scaleZChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!scaleChange.z && scale.z != endScaleZChange)
		{
			this->scaleChange.z = true;               // スケールZを変える
			this->startScaleChange.z = this->scale.z;      // スケールZの変更の開始の値
			this->endScaleChange.z = endScaleZChange;    // ここまでスケールZを変える
			this->scaleChangeTime.z = scaleZChangeTime;   // スケールZを変える時間
			this->scaleChangeEasing.z = scaleZChangeEasing; // そのイージングにするか
			this->scaleChangeElapsedTime.z = 0.0f;               // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->scaleChangeTime.z <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// スケールのリセットの値を設定
	void SettingsScaleReset(DirectX::XMFLOAT3& scaleReset) { this->scaleReset = scaleReset; }

	// スケールのリセット
	void SetScaleReset() { this->scale = scaleReset; }

	// スケールリセットの設定
	void SetScaleResetChange(float scaleResetChangeTime = 0.3f,
		ScaleChangeEasing::Type scaleChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		//! スケール変更を解除
		scaleChange.all = false;
		scaleChange.x = false;
		scaleChange.y = false;
		scaleChange.z = false;

		if ((this->scale.x != this->scaleReset.x
			|| this->scale.y != this->scaleReset.y
			|| this->scale.z != this->scaleReset.z))
		{
			this->scaleChange.all = true;                 // スケールを変える
			this->startScaleChange = this->scale;          // スケールの変更の開始の値
			this->endScaleChange = scaleReset;           // ここまでスケールを変える
			this->scaleChangeTime.all = scaleResetChangeTime; // スケールを変える時間
			this->scaleChangeEasing.all = scaleChangeEasing;    // どのイージングにするか
			this->scaleChangeElapsedTime.all = 0.0f;                 // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->scaleChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

#endif

//---------------------------------------------------------------------------//

	/*! タイマー */
#if 1

	// ムービー中のアニメーション設定
	void SetMovieAnimation(int movieAnimNum, bool movieAnimLoop)
	{
		this->movieAnimation = false;         // アニメーションを設定できるようにする
		this->movieAnimNum   = movieAnimNum;  // アニメーションの番号
		this->movieAnimLoop  = movieAnimLoop; // アニメーションをループさせるか
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

	// アクション終了までの待ち時間を設定
	void SetActionFinishWaitTimer(float actionFinishWaitTimer) { this->actionFinishWaitTimer = actionFinishWaitTimer; }

	// アクション終了までの待ち時間の初期状態を設定
	void SetActionFinishWaitInitialTimer(float actionFinishWaitInitialTimer) { this->actionFinishWaitInitialTimer = actionFinishWaitInitialTimer; }

	// 無敵時間設定
	void SetInvincibleTimer(float invincibleTimer) { this->invincibleTimer = invincibleTimer; }

#endif

//---------------------------------------------------------------------------//

	/*! 当たり判定 */
#if 1

	// 当たり判定のタイプを設定
	void SetCollisionType(CollisionType collisionType) { this->collisionType = collisionType; }

	/*! 当たり判定のオフセット */
#if 1
	// 当たり判定のオフセットの設定
	void SetCollisionOffset(const DirectX::XMFLOAT3& collisionOffset) { this->collisionOffset = collisionOffset; }

	// 当たり判定のオフセットXを設定
	void SetCollisionOffsetX(float collisionOffsetX) { this->collisionOffset.x = collisionOffsetX; }

	// 当たり判定のオフセットYを設定
	void SetCollisionOffsetY(float collisionOffsetY) { this->collisionOffset.y = collisionOffsetY; }

	// 当たり判定のオフセットZを設定
	void SetCollisionOffsetZ(float collisionOffsetZ) { this->collisionOffset.z = collisionOffsetZ; }

#endif

	// 当たり判定のオフセットのリセットの値を設定
	void SettingsCollisionOffsetReset(DirectX::XMFLOAT3& collisionOffsetReset) { this->collisionOffsetReset = collisionOffsetReset; }

	// 当たり判定のオフセットのリセット
	void SetCollisionOffsetReset() { this->collisionOffset = this->collisionOffsetReset; }

	// 当たり判定の位置調整変更の設定
	void SetCollisionOffsetChange(const DirectX::XMFLOAT3& endCollisionOffsetChange, float collisionOffsetChangeTime)
	{
		if (!this->collisionOffsetChange &&
			(collisionOffset.x != endCollisionOffsetChange.x
				|| collisionOffset.y != endCollisionOffsetChange.y
				|| collisionOffset.z != endCollisionOffsetChange.z))
		{
			this->collisionOffsetChange = true;                      // スケールを変える
			this->startCollisionOffsetChange = this->collisionOffset;     // スケールの変更の開始の値
			this->endCollisionOffsetChange = endCollisionOffsetChange;  // ここまでスケールを変える
			this->collisionOffsetChangeTime = collisionOffsetChangeTime; // スケールを変える時間
			this->collisionOffsetChangeElapsedTime = 0.0f;                      // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->collisionOffsetChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	/*! 半径 */
#if 1

	// 半径設定
	void SetRadius(float radius) { this->radius = radius; }

	// 半径変更の設定
	void SetRadiusChange(float endRadiusChange, float radiusChangeTime)
	{
		if (!radiusChange && endRadiusChange != this->endRadiusChange)
		{
			this->radiusChange = true;              // 半径を変える
			this->startRadiusChange = this->radius;      // 半径の変更の開始の値
			this->endRadiusChange = endRadiusChange;   // ここまで半径を変える
			this->radiusChangeTime = radiusChangeTime;  // 半径を変える時間
			this->radiusChangeElapsedTime = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->radiusChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 半径変更の解除
	void SetRadiusChangeUnlock() { this->radiusChange = false; }

	// 半径のリセットの値を設定
	void SettingsRadiusReset(float radiusReset) { this->radiusReset = radiusReset; }

	// 半径のリセット
	void SetRadiusReset() { this->radius = radiusReset; }

	// 半径リセットの設定
	void SetRadiusResetChange(float radiusChangeTime = 0.3f)
	{
		this->radiusChange = false; // 半径変更を解除

		if (this->radius != this->radiusReset)
		{
			this->radiusChange = true;              // 半径を変える
			this->startRadiusChange = this->radius;      // 半径の変更の開始の値
			this->endRadiusChange = this->radiusReset; // ここまで半径を変える
			this->radiusChangeTime = radiusChangeTime;  // 半径を変える時間
			this->radiusChangeElapsedTime = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->radiusChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

#endif

	/*! 高さ */
#if 1

	// 高さ設定
	void SetHeight(float height) { this->height = height; }

	// 高さ変更の設定
	void SetHeightChange(float endHeightChange, float heightChangeTime)
	{
		if (!heightChange && endHeightChange != this->endHeightChange)
		{
			this->heightChange = true;              // 高さを変える
			this->startHeightChange = this->height;      // 高さの変更の開始の値
			this->endHeightChange = endHeightChange;   // ここまで高さを変える
			this->heightChangeTime = heightChangeTime;  // 高さを変える時間
			this->heightChangeElapsedTime = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->heightChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 高さ変更の解除
	void SetHeightChangeUnlock() { this->heightChange = false; }

	// 高さのリセットの値を設定
	void SettingsHeightReset(float heightReset) { this->heightReset = heightReset; }

	// 高さのリセット
	void SetHeightReset() { this->height = heightReset; }

	// 高さリセットの設定
	void SetHeightResetChange(float heightChangeTime = 0.3f)
	{
		this->heightChange = false; // 高さ変更を解除

		if (this->height != this->heightReset)
		{
			this->heightChange = true;              // 高さを変える
			this->startHeightChange = this->height;      // 高さの変更の開始の値
			this->endHeightChange = this->heightReset; // ここまで高さを変える
			this->heightChangeTime = heightChangeTime;  // 高さを変える時間
			this->heightChangeElapsedTime = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->heightChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

#endif

	/*! 幅 */
#if 1

	// 幅の設定
	void SetWidth(float width) { this->width = width; }

	// 幅変更の設定
	void SetWidthChange(float endHeightChange, float heightChangeTime)
	{
		if (!widthChange && endWidthChange != this->endWidthChange)
		{
			this->widthChange = true;             // 幅を変える
			this->startWidthChange = this->width;      // 幅の変更の開始の値
			this->endWidthChange = endWidthChange;   // ここまで幅を変える
			this->widthChangeTime = widthChangeTime;  // 幅を変える時間
			this->widthChangeElapsedTime = 0.0f;             // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->widthChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

	// 幅変更の解除
	void SetWidthChangeUnlock() { this->widthChange = false; }

	// 幅のリセットの値を設定
	void SettingsWidthReset(float widthReset) { this->widthReset = widthReset; }

	// 幅のリセット
	void SetWidthReset() { this->width = widthReset; }

	// 幅リセットの設定
	void SetWidthResetChange(float widthChangeTime = 0.3f)
	{
		this->widthChange = false; // 幅変更を解除

		if (this->width != this->widthReset)
		{
			this->widthChange = true;             // 幅を変える
			this->startWidthChange = this->width;      // 幅の変更の開始の値
			this->endWidthChange = this->widthReset; // ここまで幅を変える
			this->widthChangeTime = widthChangeTime;  // 幅を変える時間
			this->widthChangeElapsedTime = 0.0f;             // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->widthChangeTime <= 0)
			{
				throw std::out_of_range(u8"イージング出来ません！！");
			}
		}
	}

#endif

	// 奥行の設定
	void SetDepth(float depth) { this->depth = depth; }

	// 奥行のリセット
	void SetDepthReset() { this->depth = depthReset; }

#endif

//---------------------------------------------------------------------------//

	/*! 敵関連 */
#if 1

	// モデルの設定
	void SetModel(const std::string& modelPath) { model = std::make_unique<Model>(modelPath.c_str()); }

	// インデックスを指定してモデルを設定
	void SetModels(int modelNum, const std::string& modelPath) { models[modelNum] = std::make_unique<Model>(modelPath.c_str()); }

	// ターゲットの位置を設定
	void SetTargetPosition(const DirectX::XMFLOAT3& targetPosition) { this->targetPosition = targetPosition; }

	// 接触ダメージをするかの設定
	void SetIsContactDamage(bool isContactDamage) { this->isContactDamage = isContactDamage; }

	// 接触ダメージの設定
	void SetContactDamage(int contactDamage) { this->contactDamage = contactDamage; }

	// 接触ダメージのリセット
	void SetContactDamageReset() { this->contactDamage = contactDamageReset; }

#endif

//---------------------------------------------------------------------------//
	/*! カメラ */
#if 1

	// カメラのターゲットの位置変更の設定
	void SetCameraTargetChange(DirectX::XMFLOAT3 endCameraTarget, float cameraTargetChangeTime, CameraTargetChangeEasing cameraTargetChangeEasing = CameraTargetChangeEasing::Linear);

	// カメラのターゲットの位置のリセットの設定
	void SetCameraTargetResetChange(float cameraTargetResetChangeTime = 0.5f, CameraTargetChangeEasing cameraTargetChangeEasing = CameraTargetChangeEasing::Linear);

	// カメラの角度変更の設定
	void SetCameraAngleChange(DirectX::XMFLOAT3 endCameraAngle, float cameraAngleChangeTime, CameraAngleChangeEasing cameraAngleChangeEasing = CameraAngleChangeEasing::Linear);

	// カメラの角度のリセットの設定
	void SetCameraAngleResetChange(float cameraAngleResetChangeTime = 0.5f, CameraAngleChangeEasing cameraAngleChangeEasing = CameraAngleChangeEasing::Linear);

	// カメラの範囲の変更の設定
	void SetCameraRangeChange(float endCameraRange, float cameraRangeChangeTime, CameraRangeChangeEasing cameraRangeChangeEasing = CameraRangeChangeEasing::Linear);

	// カメラの範囲のリセットの数値
	void SetCameraRangeResetChange(float cameraRangeResetChangeTime = 0.5f, CameraRangeChangeEasing cameraRangeChangeEasing = CameraRangeChangeEasing::Linear);

	// カメラシェイク設定
	void SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower);
#endif

//---------------------------------------------------------------------------//
	/*! シェーダー */

	//! ポストエフェクト
#if 1
	// ポストエフェクトのステータスを設定
	void SetPostEffectStatus(float contrast = 1.0f,
		float saturation = 0.8f,
		const DirectX::XMFLOAT3& colorFilter = { 1.2f, 1.3f, 1.35f },
		float chromaticAberration = 0);

	// ポストエフェクトのステータスをリセットする
	void SetPostEffectStatusResetChange(
		float contrastChangeTime = 1.5f,
		float saturationChangeTime = 1.5f,
		float colorFilterChangeTime = 1.5f,
		float chromaticAberrationChangeTime = 1.5f,
		float blurStrengthChangeTime = 1.0f);

	/*! コントラスト */
#if 1
	// コントラストの設定
	void SetContrast(float contrast);

	// コントラスト変更の設定
	void SetContrastChange(float endContrastChange, float contrastChangeTime = 1.5f);

	// コントラストのリセット
	void SetContrastReset();

	// コントラストリセット変更の設定
	void SetContrastResetChange(float contrastChangeTime = 1.5f);

#endif

	/*! サチュレーション */
#if 1

	// サチュレーション(色の彩度)の設定
	void SetSaturation(float saturation);

	// サチュレーション(色の彩度)変更の設定
	void SetSaturationChange(float endSaturationChange, float saturationChangeTime = 1.5f);

	// サチュレーションのリセット
	void SetSaturationReset();

	// サチュレーションリセットの変更の設定
	void SetSaturationResetChange(float saturationChange = 1.5f);

#endif

	/*! カラーフィルター */
#if 1

	// カラーフィルターの設定
	void SetColorFilter(const DirectX::XMFLOAT3& colorFilter);

	// カラーフィルター変更の設定
	void SetColorFilterChange(DirectX::XMFLOAT3& endColorFilterChange, float colorFilterChangeTime = 1.5f);

	// カラーフィルターのリセット
	void SetColorFilterReset();

	// カラーフィルターリセットの変更の設定
	void SetColorFilterResetChange(float colorFilterChangeTime = 1.5f);

#endif

	/*! クロマティックアベレーション */
#if 1

	// クロマティックアベレーション(色収差(色ズレ))の設定
	void SetChromaticAberration(float chromaticAberration);

	// クロマティックアベレーション(色収差(色ズレ))変更の設定
	void SetChromaticAberrationChange(float endChromaticAberrationChange, float chromaticAberrationChangeTime = 1.5f);

	// クロマティックアベレーションのリセット
	void SetChromaticAberrationReset();

	// クロマティックアベレーションリセットの設定
	void SetChromaticAberrationResetChange(float chromaticAberrationChangeTime = 1.5f);

#endif

	/*! ブラーの強度 */
#if 1

	// ブラーの強度を設定
	void SetBlurStrength(float blurStrength);

	// ブラーの強度変更の設定
	void SetBlurStrengthChange(float endBlurStrengthChange, float blurStrengthChangeTime = 1.0f);

	// ブラーの強度のリセット
	void SetBlurStrengthReset();

	// ブラーの強度リセットの設定
	void SetBlurStrengthResetChange(float blurStrengthChangeTime = 1.0f);

#endif

	// カメラシェイク(シェーダー)の設定
	void SetCameraShakeShader(float cameraShakeTime = 1.0f, float cameraShakeStrength = 0.2f, DirectX::XMFLOAT2 cameraShakeOffset = { 0, -0.05f }, float cameraShakeSpeed = 50.0f);

	// カメラシェイク(シェーダー)の解除
	void SetCameraShakeShaderUnlock();

#endif

#endif // セッター

/**************************************************************************************/
	/*! ゲッター */
#if 1

	/*! HP */
#if 1

	// HPを取得
	int GetHp() const { return hp; }

	// HPダメージ取得
	int GetHpDamage() const { return hpDamage; }

	// 最大HPを取得
	int GetMaxHp() const { return maxHp; }

	// HP処理終了取得
	bool GetHpDirectorFinished() const { return hpDirectorFinished; }

#endif

//---------------------------------------------------------------------------//

	/*! タイマー */
#if 1

	// ステート変更までの時間を取得
	float GetStateChangeWaitTimer()  const { return stateChangeWaitTimer; }

	// アクションタイマーを取得
	float GetActionTimer() const { return actionTimer; }

	// アクション終了までの待ち時間を取得
	float GetActionFinishWaitTimer() const { return actionFinishWaitTimer; }

	// アクション終了までの待ち時間の初期状態を取得
	float GetActionFinishWaitInitialTimer() const { return actionFinishWaitInitialTimer; }

	// ヒットストップ時間の取得
	float GetHitStopElapsedTime() const { return hitStopElapsedTime; }

	// 無敵時間取得
	float GetInvincibleTimer() const { return invincibleTimer; }

	// 防御力アップ時間取得
	float GetDefensesUpTimer() const { return defensesUpTimer; }

#endif

//---------------------------------------------------------------------------//

	/*! ムービー */
#if 1

	// ムービー中かどうかを取得
	bool GetMovieScene() const { return movieScene; }

	// ムービー時間取得
	float GetMovieTime() const { return movieTime; }

#endif

//---------------------------------------------------------------------------//

	/*! 描画関連 */
#if 1

	// 不透明度を取得
	float GetOpacity() const { return opacity; }

	// 不透明度を変えているかを取得
	bool GetOpacityChange() const { return opacityChange; }

	// エミシッブの色を取得
	const DirectX::XMFLOAT3& GetEmissiveColor() const { return emissiveColor; }

	// エミッシブの色を変えているかを取得
	bool GetEmissiveColorChange() const { return emissiveColorChange; }

	// エミシッブの強さを取得
	float GetEmissiveStrength() const { return emissiveStrength; }

	// エミッシブの強さを変えているかを取得
	bool GetEmissiveStrengthChange() const { return emissiveStrengthChange; }

	// エミシッブの形態変化演出をしているかを取得
	bool GetEmissivePhaseChange() const { return emissivePhaseChange; }

#endif

//---------------------------------------------------------------------------//

	/*! ポストエフェクト */
#if 1

	// コントラストを変更しているか取得
	bool GetContrastChange();

	// サチュレーション(色の彩度)を変更しているかを取得
	bool GetSaturationChange();

	// カラーフィルターを変更しているかを取得
	bool GetColorFilterChange();

	// クロマティックアベレーション(色収差(色ズレ))を変更しているかを取得
	bool GetChromaticAberrationChange();

	// ブラーの強度を変更しているか取得
	bool GetBlurStrengthChange();

#endif

//---------------------------------------------------------------------------//

	// 壁に触れているか
	bool IsWall() const { return isWall; }

	// 地面に接地しているか
	bool IsGround() const { return isGround; }

	// トランスフォーム取得
	const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	// 方向取得
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }

	/*! 位置 */
#if 1

	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// 位置変更をしているかを取得
	bool GetPositionAllChange() const { return positionChange.all; }

	// 位置X変更をしているかを取得
	bool GetPositionXChange() const { return positionChange.x; }

	// 位置Y変更をしているかを取得
	bool GetPositionYChange() const { return positionChange.y; }

	// 位置Z変更をしているかを取得
	bool GetPositionZChange() const { return positionChange.z; }

#endif

	// 加速度取得
	const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }

//---------------------------------------------------------------------------//

	/*! 角度 */
#if 1

	// 角度取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	// 角度変更をしているかを取得
	bool GetAngleAllChange() const { return angleChange.all; }

	// 角度X変更をしているかを取得
	bool GetAngleXChange() const { return angleChange.x; }

	// 角度Y変更をしているかを取得
	bool GetAngleYChange() const { return angleChange.y; }

	// 角度Z変更をしているかを取得
	bool GetAngleZChange() const { return angleChange.z; }

	// 回転しているかを取得
	bool GetAngleRotation() const { return angleRotation; }

#endif

//---------------------------------------------------------------------------//

	/*! スケール */
#if 1

	// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	// スケールリセットの取得
	const DirectX::XMFLOAT3& GetScaleReset() const { return scaleReset; }

	// スケール変更をしているかを取得
	bool GetScaleAllChange() const { return scaleChange.all; }

	// スケールX変更をしているかを取得
	bool GetScaleXChange() const { return scaleChange.x; }

	// スケールY変更をしているかを取得
	bool GetScaleYChange() const { return scaleChange.y; }

	// スケールZ変更をしているかを取得
	bool GetScaleZChange() const { return scaleChange.z; }

#endif

//---------------------------------------------------------------------------//

	/*! 当たり判定 */
#if 1

	// 当たり判定のタイプ取得
	CollisionType GetCollisionType() const { return collisionType; }

	// 当たり判定の位置取得
	const DirectX::XMFLOAT3& GetCollisionPos() const { return collisionPos; }

	// 当たり判定の調整を取得
	const DirectX::XMFLOAT3& GetCollisionOffset() const { return collisionOffset; }

	// 半径取得
	float GetRadius() const { return radius; }

	// 高さ取得
	float GetHeight() const { return height; }

	// 横の長さを取得
	float GetWidth() const { return width; }

	// 奥行きを取得
	float GetDepth() const { return depth; }

#endif

#endif // ゲッター

protected:

//!\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
	/*! 更新処理など */

/*! HP関連 */
#if 1

	// HP管理
	void HpControll(float elapsedTime);

	// HP演出
	bool HpDirector(float elapsedTime);

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

protected:

	//! オブジェクト
	enum class Object
	{
		Character,  // キャラクター
		Stage,      // ステージ
		Projectile, // 飛び道具
	}object;

protected:

	//! 状態更新処理
	void UpdateGameObjectBaseState(float elapsedTime, Object object = Object::Character);

	// 状態更新処理関連(上の関数にまとめられている)
#if 1

	// ヒットストップ更新処理
	bool UpdateHitStop(float elapsedTime);

	// 行列更新処理
	void UpdateTransform();

	// 行列更新処理(ステージ)
	void UpdateTransform_Stage();

	// 行列更新処理(飛び道具)
	void UpdateTransform_Projectile();

	// レイキャスト更新処理
	void UpdateProjectileRaycast();

	/*! 速力処理 */
#if 1
	//! 加速度更新処理
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

	/*! 位置*/
#if 1
	// 位置全体変更更新処理
	bool UpdatePositionAllChange(float elapsedTime);

	// 位置X変更更新処理
	bool UpdatePositionXChange(float elapsedTime);

	// 位置Y変更更新処理
	bool UpdatePositionYChange(float elapsedTime);

	// 位置Z変更更新処理
	bool UpdatePositionZChange(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! 角度 */
#if 1

	// 角度制限処理
	void LimitAngle();

	// 角度全体変更更新処理
	bool UpdateAngleChange(float elapsedTime);

	// 角度X変更更新処理
	bool UpdateAngleXChange(float elapsedTime);

	// 角度Y変更更新処理
	bool UpdateAngleYChange(float elapsedTime);

	// 角度Z変更更新処理
	bool UpdateAngleZChange(float elapsedTime);

	// 角度の回転の更新処理(特定の値は目指さず、指定された時間中に回転する)
	bool UpdateAngleRotation(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! スケール */
#if 1

	// スケール全体変更更新処理
	bool UpdateScaleChange(float elapsedTime);

	// スケールX変更更新処理
	bool UpdateScaleXChange(float elapsedTime);

	// スケールY変更更新処理
	bool UpdateScaleYChange(float elapsedTime);

	// スケールZ変更更新処理
	bool UpdateScaleZChange(float elapsedTime);

#endif

	//---------------------------------------------------------------------------//

		/*! 描画関連 */
#if 1
	// マテリアルの色変更更新処理
	bool UpdateMaterialColorChange(float elapsedTime);

	// 不透明度変更更新処理
	bool UpdateOpacityChange(float elapsedTime);

	// エミッシブの色変更更新処理
	bool UpdateEmissiveColorChange(float elapsedTime);

	//エミッシブの強さ変更更新処理
	bool UpdateEmissiveStrengthChange(float elapsedTime);

	//エミッシブの形態変化演出更新処理
	bool UpdateEmissivePhaseChange(float elapsedTime);

	//エミッシブの色変更更新処理(形態変化)
	void UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeTime, bool& emissiveColorUp, float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! 当たり判定 */
#if 1

	// 当たり判定の位置を設定
	void CollisionPosSettings();

	// 当たり判定のオフセット変更更新処理
	bool UpdateCollisionOffsetChange(float elapsedTime);

	// 半径変更更新処理
	bool UpdateRadiusChange(float elapsedTime);

	// 高さ変更更新処理
	bool UpdateHeightChange(float elapsedTime);

	// 幅変更更新処理
	bool UpdateWidthChange(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! その他 */
#if 1

	// ムービー時間更新処理
	bool UpdateMovieTimer(float elapsedTime);

	// 無敵時間更新
	void UpdateInvincibleTimer(float elapsedTime);

	// 防御力アップ時間更新
	void UpdateDefensesUpTimer(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

#endif

	// 着地した時に呼ばれる
	virtual void OnLanding() {}

	// ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {};

	// 死亡した時に呼ばれる
	virtual void OnDead();

protected:
	// 向き
	DirectX::XMFLOAT3   direction = { 0, 0, 1 };
	// トランスフォーム
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	// オルドトランスフォーム
	DirectX::XMFLOAT4X4 oldTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	float gravity      = 0.0f; // 重力
	float gravityReset = 0.0f; // 重力のリセット

	bool isWall   = false; // 壁に触れている
	bool isGround = false; // 地面にいる

	float moveSpeed      = 10.0f; // 移動速度
	float moveSpeedReset = 10.0f; // 移動速度のリセット

	/*! 当たり判定 */
#if 1

	DirectX::XMFLOAT3 collisionPos = { 0, 0, 0 }; // 当たり判定の位置

	DirectX::XMFLOAT3 collisionOffset = { 0, 0, 0 }; // 当たり判定のオフセット
	DirectX::XMFLOAT3 collisionOffsetReset = { 0, 0, 0 }; // 当たり判定のオフセットのリセット
	bool collisionOffsetChange = false;       // 当たり判定のオフセットを変えるか

	DirectX::XMFLOAT3 startCollisionOffsetChange = { 0, 0, 0 }; // 当たり判定のオフセットの変更の開始の値
	DirectX::XMFLOAT3 endCollisionOffsetChange = { 0, 0, 0 }; // ここまで当たり判定のオフセットを変える
	float collisionOffsetChangeTime = 0.0f;        // 当たり判定のオフセットを変える時間
	float collisionOffsetChangeElapsedTime = 0.0f;        // 当たり判定のオフセット変更の経過時間

	/*! 半径 */
#if 1
	float radius = 1.0f;  // 半径
	bool  radiusChange = false; // 半径を変えるか

	float startRadiusChange = 0.0f; // 半径の変更の開始の値
	float endRadiusChange = 0.0f; // ここまで半径を変える
	float radiusChangeTime = 0.0f; // 半径を変える時間
	float radiusChangeElapsedTime = 0.0f; // 半径変更の経過時間
#endif

	/*! 高さ */
#if 1
	float height = 1.0f;  // 高さ
	bool  heightChange = false; // 高さを変えるか

	float startHeightChange = 0.0f; // 高さの変更の開始の値
	float endHeightChange = 0.0f; // ここまで高さを変える
	float heightChangeTime = 0.0f; // 高さを変える時間
	float heightChangeElapsedTime = 0.0f; // 高さ変更の経過時間
#endif

	//! ボックス
#if 1
	/*! 幅 */
#if 1
	float width = 1.0f;  // 横
	bool  widthChange = false; // 幅を変えるか

	float startWidthChange = 0.0f; // 高さの変更の開始の値
	float endWidthChange = 0.0f; // ここまで幅を変える
	float widthChangeTime = 0.0f; // 幅を変える時間
	float widthChangeElapsedTime = 0.0f; // 幅変更の経過時間
#endif

	float depth = 1.0f;  //! 奥行き

#endif

	//! リセット
	float radiusReset = 1.0f; // 半径
	float heightReset = 1.0f; // 高さ
	//! ボックス
	float widthReset = 1.0f; // 横
	float depthReset = 1.0f; // 奥行き

#endif

/*****************************************************************************************************/
	/*! 位置 */
#if 1

	DirectX::XMFLOAT3 position = { 0, 0, 0 }; // 位置
	DirectX::XMFLOAT3 startPositionChange = { 0, 0, 0 }; // 位置の変更の開始の値
	DirectX::XMFLOAT3 endPositionChange = { 0, 0, 0 }; // ここまで位置を変える

	DirectX::XMFLOAT3 velocity = { 0, 0, 0 }; // 加速度

	//! 位置を変更するか
	struct PositionChange
	{
		bool all = false; // 全ての位置を変更するか
		bool x = false; // 位置Xを変更するか
		bool y = false; // 位置Yを変更するか
		bool z = false; // 位置Zを変更するか
	}positionChange;

	//! 位置を変える時間
	struct PositionChangeTime
	{
		float all = 0.0f; // 全ての位置を変える時間
		float x = 0.0f; // 位置Xを変える時間
		float y = 0.0f; // 位置Yを変える時間
		float z = 0.0f; // 位置Zを変える時間
	}positionChangeTime;

	//! 位置変更の経過時間
	struct PositionChangeElapsedTime
	{
		float all = 0.0f; // 全ての位置変更の経過時間
		float x = 0.0f; // 位置X変更の経過時間
		float y = 0.0f; // 位置Y変更の経過時間
		float z = 0.0f; // 位置Z変更の経過時間
	}positionChangeElapsedTime;

#endif

/*****************************************************************************************************/
	/*! 角度 */
#if 1

	DirectX::XMFLOAT3 angle = { 0, 0, 0 }; // 角度
	DirectX::XMFLOAT3 startAngleChange = { 0, 0, 0 }; // 角度の変更の開始の値
	DirectX::XMFLOAT3 endAngleChange = { 0, 0, 0 }; // ここまで角度を変える

	//! 角度を変更するか
	struct AngleChange
	{
		bool all = false; // 全ての角度を変更するか
		bool x = false; // 角度Xを変更するか
		bool y = false; // 角度Yを変更するか
		bool z = false; // 角度Zを変更するか
	}angleChange;

	//! 角度を変える時間
	struct AngleChangeTime
	{
		float all = 0.0f; // 全ての角度を変える時間
		float x = 0.0f; // 角度Xを変える時間
		float y = 0.0f; // 角度Yを変える時間
		float z = 0.0f; // 角度Zを変える時間
	}angleChangeTime;

	//! 角度変更の経過時間
	struct AngleChangeElapsedTime
	{
		float all = 0.0f; // 全ての角度変更の経過時間
		float x = 0.0f; // 角度X変更の経過時間
		float y = 0.0f; // 角度Y変更の経過時間
		float z = 0.0f; // 角度Z変更の経過時間
	}angleChangeElapsedTime;

	//! 回転(特定の値は目指さない)
	float angleRotation = false; // 角度を回転させるか
	float rotationTime = 0.0f;  // 回転時間
	DirectX::XMFLOAT3 angleRotationSpeed = { 0, 0, 0 }; // 角度の回転速度

#endif

/*****************************************************************************************************/
	/*! スケール */
#if 1 

	DirectX::XMFLOAT3 scale = { 1, 1, 1 };             // スケール
	DirectX::XMFLOAT3 scaleReset = { 0.02f, 0.02f, 0.02f }; // 何かしらの動作で大きさを変更した際に戻す用

	DirectX::XMFLOAT3 startScaleChange = { 0, 0, 0 }; // スケールの変更の開始の値
	DirectX::XMFLOAT3 endScaleChange = { 0, 0, 0 }; // ここまでスケールを変える

	//! スケールを変更するか
	struct ScaleChange
	{
		bool all = false; // 全てのスケールを変更するか
		bool x = false; // スケールXを変更するか
		bool y = false; // スケールYを変更するか
		bool z = false; // スケールZを変更するか
	}scaleChange;

	//! スケールを変える時間
	struct ScaleChangeTime
	{
		float all = 0.0f; // 全てのスケールを変える時間
		float x = 0.0f; // スケールXを変える時間
		float y = 0.0f; // スケールYを変える時間
		float z = 0.0f; // スケールZを変える時間
	}scaleChangeTime;

	//! スケール変更の経過時間
	struct ScaleChangeElapsedTime
	{
		float all = 0.0f; // 全てのスケール変更の経過時間
		float x = 0.0f; // スケールX変更の経過時間
		float y = 0.0f; // スケールY変更の経過時間
		float z = 0.0f; // スケールZ変更の経過時間
	}scaleChangeElapsedTime;

#endif

/*****************************************************************************************************/
	/*! マテリアル */
#if 1
	DirectX::XMFLOAT3 materialColor = { 1,1,1 }; // マテリアルの色
	bool materialColorChange = false;     // マテリアルの色を変更するか

	DirectX::XMFLOAT3 startMaterialColorChange = { 0, 0, 0 }; // マテリアルの色の変更の開始の値
	DirectX::XMFLOAT3 endMaterialColorChange = { 0, 0, 0 }; // ここまでマテリアルの色を変える
	float materialColorChangeTime = 0.0f;        // マテリアルの色を変える時間
	float materialColorChangeElapsedTime = 0.0f;        // マテリアルの色変更の経過時間

	//! 不透明度
	float opacity = 1.0f;  // 不透明度
	bool  opacityChange = false; // 不透明度を変更するか

	float startOpacityChange = 0.0f;  // 不透明度の変更の開始の値
	float endOpacityChange = 0.0f;  // ここまで不透明度を変える 
	float opacityChangeTime = 0.0f;  // 不透明度を変える時間
	float opacityChangeElapsedTime = 0.0f;  // 不透明度変更の経過時間
#endif

/*****************************************************************************************************/
	/*! エミッシブ */
#if 1
	DirectX::XMFLOAT3 emissiveColor = { 0.0f, 0.0f, 0.0f }; //エミッシブカラー
	bool emissiveColorChange = false;                //エミッシブの色を変えるか

	DirectX::XMFLOAT3 startEmissiveColorChange = { 0, 0, 0 }; // エミシッブの色の変更の開始の値
	DirectX::XMFLOAT3 endEmissiveColorChange = { 0, 0, 0 }; // ここまでエミシッブの色を変える
	float emissiveColorChangeTime = 0.0f;        // エミッシブの色を変える時間
	float emissiveColorChangeElapsedTime = 0.0f;        // エミッシブの色変更の経過時間

	//! エミッシブの強さ
	float emissiveStrength = 0.0f;  // エミッシブの強さ
	bool  emissiveStrengthChange = false; // エミッシブの強さを変えるか

	float startEmissiveStrengthChange = 0.0f; // エミッシブの強さの変更の開始の値
	float endEmissiveStrengthChange = 0.0f; // ここまでエミシッブの強さを変える
	float emissiveStrengthChangeTime = 0.0f; // エミシッブの強さを変える時間
	float emissiveStrengthChangeElapsedTime = 0.0f; // エミッシブの強さ変更の経過時間
#endif

//-------------------------------------------------------------------------------------------------------//

	/*! 形態変化演出(エミシッブ) */
#if 1
	bool emissivePhaseChange = false; //エミッシブの形態変化演出をするか
	float emissivePhaseChangeTimer = 0.0f;  //エミッシブの形態変化演出をする時間
	float emissiveColorMin = 0.2f;  //エミッシブの色(最小値)
	float emissiveColorMax = 1.0f;  //エミッシブの色(最大値)
	DirectX::XMFLOAT3
		emissiveColorPhaseChangeSpeed = { 4.0f, 3.8f, 4.4f }; //エミッシブの色を変える時間(形態変化演出)
	struct EmissiveColorChangeDown
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}emissiveColorChangeDown;   //エミッシブの色を減らすか(上の物とは使用用途が違う)
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

	int   hp = 10;    // HP
	int   maxHp = 5;    // 最大HP
	bool  hpDirectorFinished = false; // HPの演出が終わったか
	int   doHpDirectorCount = 0;     // HPを増やした回数(HP演出用)
	float hpDirectorElapsedTime = 0.0f;  // HP演出の経過時間

	//! HPダメージ関連(演出用)
	int   hpDamage = 5;      // HPダメージ
	float hpDamageTimer = 2;      // ダメージ演出を行うまでの時間
	float hpDamageDirectorWaitTimer = 0.005f; // HPを減らす演出にディレイをかける
	float maxhpDamageDirectorWaitTimer = 0.005f; // 最大HP減らす処理待ち時間
	float deathWaitTimer = 0;      // 消滅カウント

#endif

//-----------------------------------------------------------------------------------//
	//! HPの画像
#if 1
	DirectX::XMFLOAT2 hpSpritePos = { 0.0f, 0.0f };  // HPゲージの位置
	DirectX::XMFLOAT3 hpSpriteColor = { 1, 0, 1 };     // HPゲージの色
	DirectX::XMFLOAT2 hpImagePos = { 0.0f, 0.0f };  // HP画像の位置
	DirectX::XMFLOAT3 hpImageColor = { 1, 1, 1 };     // HP画像の色

	DirectX::XMFLOAT3 hpSpriteColorP = { 0, 1, 0 }; // HPゲージの色(プレイヤー用)

	int hpSpriteHeight = 0;     // HP画像の高さ
	float enemyHpSpriteWidth = 684.5f;  // 敵のHPゲージの長さ
	float playerHpSpriteWidth = 140.01f; // プレイヤーのHPゲージの長さ

	DirectX::XMFLOAT2 nameSpritePos = { 0, 0 }; // 名前の画像の位置
	float             nameSpriteOpacity = 0.0f;     // 名前の不透明度
#endif

//-----------------------------------------------------------------------------------//
	//! HPシェイク
#if 1
	float playerImageShakePosY = 0.0f; // プレイヤー画像シェイクの位置
	float hpSpriteShakePosY = 0.0f; // HPゲージシェイクの位置
	float hpImageShakePosY = 0.0f; // HP画像シェイクの位置
	bool  hpShake = false;// HPシェイクするか
	float hpShakeTimer = 0.0f; // HPシェイクタイマー
#endif

//-----------------------------------------------------------------------------------//
	//! HP形態変化
#if 1
	bool  hpPhaseChange = false; // HPの形態変化演出
	float hpPhaseChangeTimer = 0.0f;  // HPの形態変化演出を行う時間

	//! HP画像の色変更
	float hpImageColorMin = 0.2f;                   // HP画像の色(最小値)
	float hpImageColorMax = 1.0f;                   // HP画像の色(最大値)
	DirectX::XMFLOAT3
		hpImageColorChangeSpeed = { 4.0f, 3.8f, 4.4f }; // HP画像の色を変える時間
	struct HpImageColorUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}hpImageColorUp;    // HP画像の色を増やすか

	//! HPゲージの色変更
	float hpSpriteColorMin = 0;     // HPゲージの色(最小値)
	float hpSpriteColorMax = 1.0f;  // HPゲージの色(最大値)
	float hpSpriteColorChangeSpeed = 3.8f;  // HPゲージの色を変える時間
	bool hpSpriteColorDown = false; // HPゲージの色を下げるか
#endif

//-----------------------------------------------------------------------------------//
	//! ダメージ計算関連
#if 1
	bool  isGuard = false; // ガードしているか
	int   damage = 0;     // ダメージ
	int   additionalDamage = 0;     // 追加ダメージ(防御力低下などを表現する)
	int   defenses = 0;     // 防御力
	float defensesUpTimer = 0.0f;    // 防御力アップ時間
#endif

//-----------------------------------------------------------------------------------//

	/*! 敵関連 */
#if 1

	std::unique_ptr<Model> model;     // モデル
	std::unique_ptr<Model> models[8]; // モデル(構造体)

	std::unique_ptr<Effect> destroyEffect; // 破棄エフェクト

	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };     // ターゲットの位置
	float turnSpeed = DirectX::XMConvertToRadians(360); // 回転速度

	DirectX::XMFLOAT3 debugPrimitiveColor = { 0.5f, 0.5f, 0.5f }; // デバッグプリミティブの色

	bool isContactDamage = false; // 接触ダメージをするか
	int  contactDamage = 12;    // 接触ダメージ【ゲーム中にダメージを変えたい場合は引数にこの変数を割り当てる】
	int  contactDamageReset = 12;    // 接触ダメージのリセット

#endif

	//-----------------------------------------------------------------------------------//
		//! その他
#if 1
	bool  hideSprites = false; // HPなどを隠す
	float invincibleTimer = 0.0f;  // 無敵時間

	float stateChangeWaitTimer = 0.0f; // ステート切り替えまでの時間
	float actionTimer = 0.0f; // アクションタイマー
	float actionFinishWaitTimer = 0.0f; // アクション終了までの待ち時間
	float actionFinishWaitInitialTimer = 0.0f; // アクション終了までの待ち時間の初期状態
#endif

/*****************************************************************************************************/
	/*! ヒットストップ */
#if 1
	bool  hitStop = false; // ヒットストップ中か
	bool  hitStopCameraShake = false; // ヒットストップでカメラを揺らすか
	float hitStopElapsedTime = 1.0f;  // ヒットストップ込みのelapsedTime
	float hitStopSeconds = 0.0f;  // ヒットストップの時間
	float hitStopMaxSeconds = 0.3f;  // ヒットストップの最大の時間
#endif

	/*****************************************************************************************************/
	float friction = 0.5f; // 摩擦
	float acceleration = 3.0f; // 加速力
	float maxMoveSpeed = 5.0f; // 最大移動速度
	float moveVecX = 0.0f; // 移動ベクトルX
	float moveVecZ = 0.0f; // 移動ベクトルZ

	float stepOffset = 1.0f; // レイキャストのオフセット
};