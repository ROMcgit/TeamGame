#include "GameObjectBase.h"
#include "Game/Stage/StageManager.h"

#include "Game/Camera/Camera.h"
#include "Game/Camera/CameraController.h"

#include "Input/Input.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Other/Easing.h"
#include <imgui.h>
#include <random>

// 死亡した時に呼ばれる
void GameObjectBase::OnDead()
{
}

/*****************************************************************************************/

/*! HP */
#if 1

//! HP管理
void GameObjectBase::HpControll(float elapsedTime)
{
	// HPが上限を超えないように
	if (hp > maxHp)       hp = maxHp;
	if (hpDamage > maxHp) hpDamage = maxHp;

	// ピンチなら色を変える(プレイヤー)
	//if (hp < maxHp * 0.25f) hpSpriteColorP = { 1.0f, 0.4118f, 0.7059f };

	// ダメージ処理
	if (hpDamage > hp)
	{
		hpDamageTimer += elapsedTime;
		if (hpDamageTimer >= 0.8f)
		{
			hpDamageDirectorWaitTimer -= elapsedTime;
			if (hpDamageDirectorWaitTimer <= 0)
			{
				hpDamage--;
				hpDamageDirectorWaitTimer = maxhpDamageDirectorWaitTimer;
			}
		}
	}
	// 回復した時
	else if (hpDamage < hp) hpDamage = hp;

	// HPなどが0より下にならないようにする
	if (hp < 0) hp = 0;

	if (hpDamage == hp) hpDamageTimer = 0;
}

//! HP演出
bool GameObjectBase::HpDirector(float elapsedTime)
{
	//! 最初からHPが最大なら
	if (hp == maxHp)
		hpDirectorFinished = true;

	// HP演出が終わっているなら処理をしない
	if (hpDirectorFinished)
		return false;

	//! HP画像の生成処理
	if (hpSpriteHeight < 40)
	{
		hpSpriteHeight += 1;
		if (nameSpriteOpacity < 1.0f)
			nameSpriteOpacity += 0.04f;
	}
	else
	{
		//! 効果音
		SoundManager& sound = SoundManager::Instance();
		sound.LoadSound("HP演出", "Data/Audio/Sound/Enemy/HpDirector.wav");
		if (!sound.IsPlayingSound("HP演出"))
			sound.PlaySound("HP演出", 0.5f);

		//---------------------------------------------------------------------//

		//! 無敵にする
		invincibleTimer = 0.1f;

		//! 経過時間を計測
		hpDirectorElapsedTime += elapsedTime;

		float t = hpDirectorElapsedTime / 0.8f;

		//! HPを代入
		hp = Easing::Linear(0, maxHp, t);

		//! tが1より大きいなら
		if (t >= 1.0f)
		{
			//! 効果音を止める
			sound.StopSound("HP演出");
			hpDirectorFinished = true;
		}
	}

	return true;
}

//! HPピンチ更新処理(敵のみ)
bool GameObjectBase::UpdateHpPinchEnemy()
{
	//! HPが0なら処理を止める
	if (hp <= 0 && hpDirectorFinished)
	{
		//! エミシッブの強さを0にする
		emissiveStrength = 0.0f;
		return false;
	}

	//! HPが30%より大きいなら、処理を止める
	if (hp > maxHp * 0.3f)
		return false;

	//! HPが25%より大きいなら
	if (hp > maxHp * 0.25f)
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.1f, 0.5f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.5f);
	}
	//! HPが20%より大きいなら
	else if (hp > maxHp * 0.2f)
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.2f, 0.4f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.4f);
	}
	//! HPが9%より大きいなら
	else if (hp > maxHp * 0.09f)
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.3f, 0.3f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.3f);
	}
	else
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.7f, 0.2f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.2f);
	}

	return true;
}

//! HPシェイク
bool GameObjectBase::UpdateHpShake(float elapsedTime)
{
	// カメラシェイクタイマーが0.15f以下なら
	if (hpShakeTimer <= 0.15f && hpShake)
	{
		hpSpritePos.y = hpSpriteShakePosY - 4.5f; // HPゲージのシェイク位置
		hpSpriteColor = { 1, 0.6f, 1 };           // HPゲージの色


		hpImagePos.y = hpImageShakePosY - 4.5f;  // HP画像のシェイク位置
		hpImageColor = { 1, 0, 0 };              // HP画像の色

		hpSpriteColorP = { 1, 0, 0 };             // HPゲージを赤色にする(プレイヤー)

		hpShakeTimer += elapsedTime;

		return true;
	}
	// 元に戻す
	else
	{
		hpShakeTimer = 0.0f;
		hpShake = false;

		hpSpritePos.y = hpSpriteShakePosY; // HPゲージのシェイク位置
		hpImagePos.y = hpImageShakePosY;   // HP画像のシェイク位置

		//! HPの形態変化演出をしていないなら
		if (!hpPhaseChange)
		{
			hpSpriteColor = { 1, 0, 1 }; // HPゲージの色
			hpImageColor = { 1, 1, 1 }; // HP画像の色
			hpSpriteColorP = { 0, 1, 0 }; // HPゲージを緑色に戻す(プレイヤー)
		}

		return false;
	}
}

//! HPの形態変化演出
bool GameObjectBase::UpdateHpPhaseChange(float elapsedTime)
{
	//! 形態変化をしないなら処理を止める
	if (!hpPhaseChange)
	{
		//! HPシェイクをしていないなら
		if (!hpShake)
		{
			hpImageColor = { 1, 1, 1 }; //! 色を戻す
			hpSpriteColor = { 1, 0, 1 }; //! 紫色に戻す
		}

		return false;
	}

	//! タイマーを減らす
	if (hpPhaseChangeTimer > 0.0f)
		hpPhaseChangeTimer -= elapsedTime;
	else
		hpPhaseChange = false;

	//! HP画像の色を変える
	UpdateHpColorChange(hpImageColor.x, hpImageColorChangeSpeed.x, hpImageColorUp.x, elapsedTime);
	UpdateHpColorChange(hpImageColor.y, hpImageColorChangeSpeed.y, hpImageColorUp.y, elapsedTime);
	UpdateHpColorChange(hpImageColor.z, hpImageColorChangeSpeed.z, hpImageColorUp.z, elapsedTime);

	//! HPゲージの色を変える
	UpdateHpSpriteChange(hpSpriteColor.y, hpSpriteColorChangeSpeed, hpSpriteColorDown, elapsedTime);

	return true;
}

// HP画像の色変更処理
void GameObjectBase::UpdateHpColorChange(float& hpImageColor, float& hpImageColorChangeSpeed, bool& hpImageColorUp, float elapsedTime)
{
	//! カラー
	hpImageColor += (hpImageColorUp ? 1 : -1) * hpImageColorChangeSpeed * elapsedTime;

	//! 色が最小値より小さい、または、最大値より大きいなら
	if (hpImageColor < hpImageColorMin || hpImageColor > hpImageColorMax)
	{
		// 数値を代入
		hpImageColor = std::clamp(hpImageColor,
			hpImageColorMin, hpImageColorMax);

		hpImageColorUp = !hpImageColorUp; // フラグを反転する
	}
}

// HPゲージの色変更更新処理
void GameObjectBase::UpdateHpSpriteChange(float& hpSpriteColor, float& hpSpriteColorChangeSpeed, bool& hpSpriteColorDown, float elapsedTime)
{
	//! カラー
	hpSpriteColor += (hpSpriteColorDown ? -1 : 1) * hpSpriteColorChangeSpeed * elapsedTime;

	//! 最大なら
	if (hpSpriteColor > hpSpriteColorMax)
	{
		hpSpriteColor = hpSpriteColorMax;
		hpSpriteColorDown = true;
	}
	//! 最小なら
	else if (hpSpriteColor < hpSpriteColorMin)
	{
		hpSpriteColor = hpSpriteColorMin;
		hpSpriteColorDown = false;
	}
}

#endif

/*****************************************************************************************************************/
/*! セッター */
#if 1

/*! カメラ */
#if 1

//! 向きの設定
void GameObjectBase::SetDirection(const DirectX::XMFLOAT3& direction)
{
	DirectX::XMFLOAT3 dir = direction;

	DirectX::XMVECTOR DIR;
	DIR = DirectX::XMLoadFloat3(&dir);
	DIR = DirectX::XMVector3Normalize(DIR);
	DirectX::XMStoreFloat3(&dir, DIR);

	this->direction = dir;
}

// カメラのターゲットの位置の設定
void GameObjectBase::SetCameraTargetChange(DirectX::XMFLOAT3 endCameraTarget, float cameraTargetChangeTime, CameraTargetChangeEasing cameraTargetChangeEasing)
{
	if (!CameraController::targetChange &&
		(CameraController::target.x != endCameraTarget.x
			|| CameraController::target.y != endCameraTarget.y
			|| CameraController::target.z != endCameraTarget.z))
	{
		CameraController::targetChange = true;                                                                        // カメラのターゲットの位置を変える
		CameraController::startTargetChange = CameraController::target;											         // カメラのターゲットの変更の開始の値
		CameraController::endTargetChange = endCameraTarget;                                                             // ここまでターゲットの位置を変える
		CameraController::targetChangeTime = cameraTargetChangeTime;                                                      // ターゲットの位置を変える時間
		CameraController::targetChangeEasing = static_cast<CameraController::TargetChangeEasing>(cameraTargetChangeEasing); // どのイージングにするか
		CameraController::targetChangeElapsedTime = 0.0f;                                                                        // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (CameraController::targetChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// カメラのターゲットの位置のリセットの設定
void GameObjectBase::SetCameraTargetResetChange(float cameraTargetResetChangeTime, CameraTargetChangeEasing cameraTargetChangeEasing)
{
	//! カメラの位置の変更を解除
	CameraController::targetChange = false;

	if ((CameraController::target.x != CameraController::targetReset.x
		|| CameraController::target.y != CameraController::targetReset.y
		|| CameraController::target.z != CameraController::targetReset.z))
	{
		CameraController::targetChange = true;                                                                        // カメラのターゲットの位置を変える
		CameraController::startTargetChange = CameraController::target;											         // カメラのターゲットの変更の開始の値
		CameraController::endTargetChange = CameraController::targetReset;                                               // ここまでターゲットの位置を変える
		CameraController::targetChangeTime = cameraTargetResetChangeTime;                                                 // ターゲットの位置を変える時間
		CameraController::targetChangeEasing = static_cast<CameraController::TargetChangeEasing>(cameraTargetChangeEasing); // どのイージングにするか
		CameraController::targetChangeElapsedTime = 0.0f;                                                                        // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (CameraController::targetChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// カメラの角度変更の設定
void GameObjectBase::SetCameraAngleChange(DirectX::XMFLOAT3 endCameraAngle, float cameraAngleChangeTime, CameraAngleChangeEasing cameraAngleChangeEasing)
{
	if ((CameraController::angle.x != endCameraAngle.x)
		|| CameraController::angle.y != endCameraAngle.y
		|| CameraController::angle.z != endAngleChange.z)
	{
		CameraController::angleChange = true;                                                                      // カメラの角度を変える
		CameraController::startAngleChange = CameraController::angle;											          // カメラの角度の変更の開始の値
		CameraController::endAngleChange = endCameraAngle;                                                            // ここまで角度を変える
		CameraController::angleChangeTime = cameraAngleChangeTime;                                                     // 角度を変える時間
		CameraController::angleChangeEasing = static_cast<CameraController::AngleChangeEasing>(cameraAngleChangeEasing); // どのイージングにするか
		CameraController::angleChangeElapsedTime = 0.0f;                                                                      // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (CameraController::angleChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// カメラの角度のリセットの設定
void GameObjectBase::SetCameraAngleResetChange(float cameraAngleResetChangeTime, CameraAngleChangeEasing cameraAngleChangeEasing)
{
	//! カメラの角度の変更を解除
	CameraController::angleChange = false;

	if ((CameraController::angle.x != CameraController::angleReset.x)
		|| CameraController::angle.y != CameraController::angleReset.y
		|| CameraController::angle.z != CameraController::angleReset.z)
	{
		CameraController::angleChange = true;                                                                      // カメラの角度を変える
		CameraController::startAngleChange = CameraController::angle;											          // カメラの角度の変更の開始の値
		CameraController::endAngleChange = CameraController::angleReset;                                              // ここまで角度を変える
		CameraController::angleChangeTime = cameraAngleResetChangeTime;                                                // 角度を変える時間
		CameraController::angleChangeEasing = static_cast<CameraController::AngleChangeEasing>(cameraAngleChangeEasing); // どのイージングにするか
		CameraController::angleChangeElapsedTime = 0.0f;                                                                      // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (CameraController::angleChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// カメラの範囲の変更の設定
void GameObjectBase::SetCameraRangeChange(float endCameraRange, float cameraRangeChangeTime, CameraRangeChangeEasing cameraRangeChangeEasing)
{
	if (!CameraController::rangeChange && CameraController::range != endCameraRange)
	{
		CameraController::rangeChange = true;                                                                      // カメラの範囲を変える
		CameraController::startRangeChange = CameraController::range;											          // カメラの範囲の変更の開始の値
		CameraController::endRangeChange = endCameraRange;                                                            // ここまでカメラの範囲を変える
		CameraController::rangeChangeTime = cameraRangeChangeTime;                                                     // カメラの範囲を変える時間
		CameraController::rangeChangeEasing = static_cast<CameraController::RangeChangeEasing>(cameraRangeChangeEasing); // どのイージングにするか
		CameraController::rangeChangeElapsedTime = 0.0f;                                                                      // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (CameraController::rangeChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// カメラの範囲のリセットの数値
void GameObjectBase::SetCameraRangeResetChange(float cameraRangeResetChangeTime, CameraRangeChangeEasing cameraRangeChangeEasing)
{
	//! カメラの範囲の変更を解除
	CameraController::rangeChange = false;

	if (CameraController::range != CameraController::rangeReset)
	{
		CameraController::rangeChange = true;                                                                      // カメラの範囲を変える
		CameraController::startRangeChange = CameraController::range;											          // カメラの範囲の変更の開始の値
		CameraController::endRangeChange = CameraController::rangeReset;                                              // ここまでカメラの範囲を変える
		CameraController::rangeChangeTime = cameraRangeResetChangeTime;                                                // カメラの範囲を変える時間
		CameraController::rangeChangeEasing = static_cast<CameraController::RangeChangeEasing>(cameraRangeChangeEasing); // どのイージングにするか
		CameraController::rangeChangeElapsedTime = 0.0f;                                                                      // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (CameraController::rangeChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// カメラシェイク設定
void GameObjectBase::SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower)
{
	CameraController::cameraShake = true;             // カメラシェイクするか
	CameraController::cameraShakeTimer = cameraShakeTimer; // カメラシェイクを行う時間
	CameraController::cameraShakePower = cameraShakePower; // カメラシェイクの強さ
}

#endif

/*****************************************************************************************************************/

/*! ポストエフェクト */
#if 1

//! ポストエフェクトのステータスを設定
void GameObjectBase::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3& ColorFilter, float chromaticAberration)
{
	Camera::postEffect.contrast = contrast;            // コンストラクト
	Camera::postEffect.saturation = saturation;		  // サチュレーション
	Camera::postEffect.colorFilter = ColorFilter;		  // カラーフィルター
	Camera::postEffect.chromaticAberration = chromaticAberration; // クロマティックアベレーション
}

//! ポストエフェクトのステータスをリセットする
void GameObjectBase::SetPostEffectStatusResetChange(
	float contrastChangeTime,
	float saturationChangeTime,
	float colorFilterChangeTime,
	float chromaticAberrationChangeTime,
	float blurStrengthChangeTime)
{
	//! 変更処理を一旦止める
	Camera::contrastChange = false; // コントラスト
	Camera::saturationChange = false; // サチュレーション
	Camera::colorFilterChange = false; // カラーフィルター
	Camera::chromaticAberrationChange = false; // クロマティックアベレーション
	Camera::blurStrengthChange = false; // ブラーの強度

	//! コントラスト
	Camera::contrastChange = true;                        // コントラストの値を変える
	Camera::startContrastChange = Camera::postEffect.contrast; // コントラストの変更の開始の値
	Camera::endContrastChange = Camera::contrastReset;       // ここまでコントラストの値を変える
	Camera::contrastChangeTime = contrastChangeTime;          // コントラストの値を変える時間
	Camera::contrastChangeElapsedTime = 0.0f;                        // 経過時間をリセット

	//! サチュレーション
	Camera::saturationChange = true;                          // サチュレーションの値を変える
	Camera::startSaturationChange = Camera::postEffect.saturation; // サチュレーションの変更の開始の値
	Camera::endSaturationChange = Camera::saturationReset;       // ここまでサチュレーションの値を変える
	Camera::saturationChangeTime = saturationChangeTime;          // サチュレーションの値を変える時間
	Camera::saturationChangeElapsedTime = 0.0f;                          // 経過時間をリセット

	//! カラーフィルターの値を変更していないなら
	Camera::colorFilterChange = true;                           // カラーフィルターの値を変える
	Camera::startColorFilterChange = Camera::postEffect.colorFilter; // カラーフィルターの変更の開始の値
	Camera::endColorFilterChange = Camera::colorFilterReset;       // ここまでカラーフィルターの値を変える
	Camera::colorFilterChangeTime = colorFilterChangeTime;          // カラーフィルターの値を変える時間
	Camera::colorFilterChangeElapsedTime = 0.0f;                           // 経過時間をリセット

	//! クロマティックアベレーション
	Camera::chromaticAberrationChange = true;                                   // クロマティックアベレーションの値を変える
	Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // クロマティックアベレーションの変更の開始の値
	Camera::endChromaticAberrationChange = Camera::chromaticAberrationReset;       // ここまでクロマティックアベレーションの値を変える
	Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // クロマティックアベレーションの値を変える時間
	Camera::chromaticAberrationChangeElapsedTime = 0.0f;                                   // 経過時間をリセット

	//! ブラーの強度
	Camera::blurStrengthChange = true;                            // ブラーの強度を変える
	Camera::startBlurStrengthChange = Camera::postEffect.blurStrength; // ブラーの強度の変更の開始の値
	Camera::endBlurStrengthChange = Camera::blurStrengthReset;       // ここまでブラーの強度を変える
	Camera::blurStrengthChangeTime = blurStrengthChangeTime;          // ブラーの強度を変える時間
	Camera::blurStrengthChangeElapsedTime = 0.0f;                            // 経過時間をリセット
}

/*! コントラスト */
#if 1

// コントラストの設定
void GameObjectBase::SetContrast(float contrast)
{
	Camera::postEffect.contrast = contrast;
}

// コントラスト変更の設定
void GameObjectBase::SetContrastChange(float endContrastChange, float contrastChangeTime)
{
	if (!Camera::contrastChange && Camera::postEffect.contrast != endContrastChange)
	{
		Camera::contrastChange = true;                        // コントラストの値を変える
		Camera::startContrastChange = Camera::postEffect.contrast; // コントラストの変更の開始の値
		Camera::endContrastChange = endContrastChange;           // ここまでコントラストの値を変える
		Camera::contrastChangeTime = contrastChangeTime;          // コントラストの値を変える時間
		Camera::contrastChangeElapsedTime = 0.0f;                        // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::contrastChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// コントラストのリセット
void GameObjectBase::SetContrastReset()
{
	Camera::postEffect.contrast = Camera::contrastReset;
}

// コントラストリセット変更の設定
void GameObjectBase::SetContrastResetChange(float contrastChangeTime)
{
	//! コントラストの変更を解除
	Camera::contrastChange = false;

	if (!Camera::contrastChange)
	{
		Camera::contrastChange = true;                        // コントラストの値を変える
		Camera::startContrastChange = Camera::postEffect.contrast; // コントラストの変更の開始の値
		Camera::endContrastChange = Camera::contrastReset;       // ここまでコントラストの値を変える
		Camera::contrastChangeTime = contrastChangeTime;          // コントラストの値を変える時間
		Camera::contrastChangeElapsedTime = 0.0f;                        // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::contrastChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! サチュレーション */
#if 1

// サチュレーション(色の彩度)の設定
void GameObjectBase::SetSaturation(float saturation)
{
	Camera::postEffect.saturation = saturation;
}

// サチュレーション(色の彩度)変更の設定
void GameObjectBase::SetSaturationChange(float endSaturationChange, float saturationChangeTime)
{
	if (!Camera::saturationChange && Camera::postEffect.saturation != endSaturationChange)
	{
		Camera::saturationChange = true;                          // サチュレーションの値を変える
		Camera::startSaturationChange = Camera::postEffect.saturation; // サチュレーションの変更の開始の値
		Camera::endSaturationChange = endSaturationChange;           // ここまでサチュレーションの値を変える
		Camera::saturationChangeTime = saturationChangeTime;          // サチュレーションの値を変える時間
		Camera::saturationChangeElapsedTime = 0.0f;                          // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::saturationChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// サチュレーションのリセット
void GameObjectBase::SetSaturationReset()
{
	Camera::postEffect.saturation = Camera::saturationReset;
}

// サチュレーションリセットの変更の設定
void GameObjectBase::SetSaturationResetChange(float saturationChangeTime)
{
	//! サチュレーションの変更の解除
	Camera::saturationChange = false;

	if (!Camera::saturationChange)
	{
		Camera::saturationChange = true;                          // サチュレーションの値を変える
		Camera::startSaturationChange = Camera::postEffect.saturation; // サチュレーションの変更の開始の値
		Camera::endSaturationChange = Camera::saturationReset;       // ここまでサチュレーションの値を変える
		Camera::saturationChangeTime = saturationChangeTime;          // サチュレーションの値を変える時間
		Camera::saturationChangeElapsedTime = 0.0f;                          // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::saturationChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! カラーフィルター */
#if 1

// カラーフィルターの設定
void GameObjectBase::SetColorFilter(const DirectX::XMFLOAT3& colorFilter)
{
	Camera::postEffect.colorFilter = colorFilter;
}

// カラーフィルター変更の設定
void GameObjectBase::SetColorFilterChange(DirectX::XMFLOAT3& endColorFilterChange, float colorFilterChangeTime)
{
	if (!Camera::colorFilterChange &&
		(Camera::postEffect.colorFilter.x != endColorFilterChange.x
			|| Camera::postEffect.colorFilter.y != endColorFilterChange.y
			|| Camera::postEffect.colorFilter.z != endColorFilterChange.z))
	{
		Camera::colorFilterChange = true;                           // カラーフィルターの値を変える
		Camera::startColorFilterChange = Camera::postEffect.colorFilter; // カラーフィルターの変更の開始の値
		Camera::endColorFilterChange = endColorFilterChange;           // ここまでカラーフィルターの値を変える
		Camera::colorFilterChangeTime = colorFilterChangeTime;          // カラーフィルターの値を変える時間
		Camera::colorFilterChangeElapsedTime = 0.0f;                           // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::colorFilterChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// カラーフィルターのリセット
void GameObjectBase::SetColorFilterReset()
{
	Camera::postEffect.colorFilter = Camera::colorFilterReset;
}

// カラーフィルターリセットの変更の設定
void GameObjectBase::SetColorFilterResetChange(float colorFilterChangeTime)
{
	//! カラーフィルターの変更を解除
	Camera::colorFilterChange = false;

	if (!Camera::colorFilterChange)
	{
		Camera::colorFilterChange = true;                           // カラーフィルターの値を変える
		Camera::startColorFilterChange = Camera::postEffect.colorFilter; // カラーフィルターの変更の開始の値
		Camera::endColorFilterChange = Camera::colorFilterReset;       // ここまでカラーフィルターの値を変える
		Camera::colorFilterChangeTime = colorFilterChangeTime;          // カラーフィルターの値を変える時間
		Camera::colorFilterChangeElapsedTime = 0.0f;                           // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::colorFilterChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! クロマティックアベレーション */
#if 1

// クロマティックアベレーション(色収差(色ズレ))の設定
void GameObjectBase::SetChromaticAberration(float chromaticAberration)
{
	Camera::postEffect.chromaticAberration = chromaticAberration;
}

// クロマティックアベレーション(色収差(色ズレ))変更の設定
void GameObjectBase::SetChromaticAberrationChange(float endChromaticAberrationChange, float chromaticAberrationChangeTime)
{
	if (!Camera::chromaticAberrationChange && Camera::postEffect.chromaticAberration != endChromaticAberrationChange)
	{
		Camera::chromaticAberrationChange = true;                                   // クロマティックアベレーションの値を変える
		Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // クロマティックアベレーションの変更の開始の値
		Camera::endChromaticAberrationChange = endChromaticAberrationChange;           // ここまでクロマティックアベレーションの値を変える
		Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // クロマティックアベレーションの値を変える時間
		Camera::chromaticAberrationChangeElapsedTime = 0.0f;                                   // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::chromaticAberrationChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// クロマティックアベレーションのリセット
void GameObjectBase::SetChromaticAberrationReset()
{
	Camera::postEffect.chromaticAberration = Camera::chromaticAberrationReset;
}

// クロマティックアベレーションリセットの設定
void GameObjectBase::SetChromaticAberrationResetChange(float chromaticAberrationChangeTime)
{
	//! クロマティックアベレーションの変更を解除
	Camera::chromaticAberrationChange = false;

	if (!Camera::chromaticAberrationChange)
	{
		Camera::chromaticAberrationChange = true;                                   // クロマティックアベレーションの値を変える
		Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // クロマティックアベレーションの変更の開始の値
		Camera::endChromaticAberrationChange = Camera::chromaticAberrationReset;       // ここまでクロマティックアベレーションの値を変える
		Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // クロマティックアベレーションの値を変える時間
		Camera::chromaticAberrationChangeElapsedTime = 0.0f;                                   // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::chromaticAberrationChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! ブラーの強度 */
#if 1

// ブラーの強度を設定
void GameObjectBase::SetBlurStrength(float blurStrength)
{
	Camera::postEffect.blurStrength = blurStrength;
}

// ブラーの強度変更の設定
void GameObjectBase::SetBlurStrengthChange(float endBlurStrengthChange, float blurStrengthChangeTime)
{
	if (!Camera::blurStrengthChange && Camera::postEffect.blurStrength != endBlurStrengthChange)
	{
		Camera::blurStrengthChange = true;                            // ブラーの強度を変える
		Camera::startBlurStrengthChange = Camera::postEffect.blurStrength; // ブラーの強度の変更の開始の値
		Camera::endBlurStrengthChange = endBlurStrengthChange;           // ここまでブラーの強度を変える
		Camera::blurStrengthChangeTime = blurStrengthChangeTime;          // ブラーの強度を変える時間
		Camera::blurStrengthChangeElapsedTime = 0.0f;                            // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::blurStrengthChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

// ブラーの強度のリセット
void GameObjectBase::SetBlurStrengthReset()
{
	Camera::postEffect.blurStrength = Camera::blurStrengthReset;
}

// ブラーの強度リセットの設定
void GameObjectBase::SetBlurStrengthResetChange(float blurStrengthChangeTime)
{
	//! ブラーの強度の変更を解除
	Camera::blurStrengthChange = false;

	if (!Camera::blurStrengthChange)
	{
		Camera::blurStrengthChange = true;                            // ブラーの強度を変える
		Camera::startBlurStrengthChange = Camera::postEffect.blurStrength; // ブラーの強度の変更の開始の値
		Camera::endBlurStrengthChange = Camera::blurStrengthReset;       // ここまでブラーの強度を変える
		Camera::blurStrengthChangeTime = blurStrengthChangeTime;          // ブラーの強度を変える時間
		Camera::blurStrengthChangeElapsedTime = 0.0f;                            // 経過時間をリセット

		// 変更時間の設定が0以下の時にエラーを出す
		if (Camera::blurStrengthChangeTime <= 0)
		{
			throw std::out_of_range("イージング出来ません！！");
		}
	}
}

#endif

// カメラシェイク(シェーダー)の設定
void GameObjectBase::SetCameraShakeShader(float cameraShakeTime, float cameraShakeStrength, DirectX::XMFLOAT2 cameraShakeOffset, float cameraShakeSpeed)
{
	if (!Camera::cameraShakeShader)
	{
		Camera::cameraShakeShader = true;                // カメラシェイク(シェーダー)をする
		Camera::cameraShakeShaderTime = cameraShakeTime;     // カメラシェイク(シェーダー)の時間
		Camera::postEffect.shakeStrength = cameraShakeStrength; // カメラシェイク(シェーダー)の強さ
		Camera::postEffect.shakeOffset = cameraShakeOffset;   // カメラシェイク(シェーダー)のずらす位置
		Camera::postEffect.shakeSpeed = cameraShakeSpeed;    // カメラシェイク(シェーダー)の速さ
	}
}

// カメラシェイク(シェーダー)の解除
void GameObjectBase::SetCameraShakeShaderUnlock()
{
	Camera::cameraShakeShader = false; // カメラシェイク(シェーダー)を解除
	Camera::postEffect.shakeTime = 0.0f;  // カメラシェイクの計測時間を0にする
	Camera::postEffect.shakeStrength = 0.0f;  // カメラシェイクの強さを0にする
}

//----------------------------------------------------------------------------------------------------------------//

/*! ゲッター */
#if 1

// コントラストを変更しているか取得
bool GameObjectBase::GetContrastChange()
{
	return Camera::contrastChange;
}

// サチュレーション(色の彩度)を変更しているかを取得
bool GameObjectBase::GetSaturationChange()
{
	return Camera::saturationChange;
}

// カラーフィルターを変更しているかを取得
bool GameObjectBase::GetColorFilterChange()
{
	return Camera::colorFilterChange;
}

// クロマティックアベレーション(色収差(色ズレ))を変更しているかを取得
bool GameObjectBase::GetChromaticAberrationChange()
{
	return Camera::chromaticAberrationChange;
}

// ブラーの強度を変更しているか取得
bool GameObjectBase::GetBlurStrengthChange()
{
	return Camera::blurStrengthChange;
}

#endif // ゲッター

#endif // ポストエフェクト

#endif // セッター

/*****************************************************************************************************************/

//! 状態更新処理
void GameObjectBase::UpdateGameObjectBaseState(float elapsedTime, Object object)
{
	// ヒットストップ更新処理
	UpdateHitStop(elapsedTime);

	switch (object)
	{
	//! キャラクター
	case Object::Character:
	{
		//! 行列更新処理
		UpdateTransform();

		//! 速力更新処理
		UpdateVelocity(hitStopElapsedTime);
	}
	break;
	//! ステージ
	case Object::Projectile:
	{
		//! 行列更新処理(飛び道具)
		UpdateTransform_Projectile();

		//! レイキャスト更新処理(飛び道具)
		UpdateProjectileRaycast();
	}
	break;
	//! 飛び道具
	case Object::Stage:
	{
		//! 行列更新処理(ステージ)
		UpdateTransform_Stage();
	}
	break;
	default:
		break;
	}

	/*! HP */
#if 1

	// HP管理
	HpControll(elapsedTime);

	// HPシェイク	更新処理
	UpdateHpShake(elapsedTime);

	// HPの形態変化演出
	UpdateHpPhaseChange(elapsedTime);

#endif

//---------------------------------------------------------//

	/*! 位置 */
#if 1

	// 位置変更更新処理
	UpdatePositionAllChange(hitStopElapsedTime);

	// 位置X変更更新処理
	UpdatePositionXChange(hitStopElapsedTime);

	// 位置Y変更更新処理
	UpdatePositionYChange(hitStopElapsedTime);

	// 位置Z変更更新処理
	UpdatePositionZChange(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	/*! 角度 */
#if 1

	// 角度制限処理
	LimitAngle();

	// 角度全体変更更新処理
	UpdateAngleChange(hitStopElapsedTime);

	// 角度X変更更新処理
	UpdateAngleXChange(hitStopElapsedTime);

	// 角度Y変更更新処理
	UpdateAngleYChange(hitStopElapsedTime);

	// 角度Z変更更新処理
	UpdateAngleZChange(hitStopElapsedTime);

	// 角度の回転の更新処理(特定の値は目指さず、指定された時間中に回転する)
	UpdateAngleRotation(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	/*! スケール */
#if 1

	// スケール変更更新処理
	UpdateScaleChange(hitStopElapsedTime);

	// スケールX変更更新処理
	UpdateScaleXChange(hitStopElapsedTime);

	// スケールY変更更新処理
	UpdateScaleYChange(hitStopElapsedTime);

	// スケールZ変更更新処理
	UpdateScaleZChange(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	/*! 描画関連 */
#if 1

	// マテリアルの色変更更新処理
	UpdateMaterialColorChange(elapsedTime);

	// 不透明度変更処理
	UpdateOpacityChange(elapsedTime);

	//エミッシブの色変更更新処理
	UpdateEmissiveColorChange(elapsedTime);

	//エミッシブの強さ変更更新処理
	UpdateEmissiveStrengthChange(elapsedTime);

	//エミッシブの形態変化演出更新処理
	UpdateEmissivePhaseChange(elapsedTime);

#endif

//---------------------------------------------------------//

	/*! 当たり判定 */
#if 1

	// 当たり判定の位置を設定
	CollisionPosSettings();

	// 当たり判定のオフセット変更更新処理
	UpdateCollisionOffsetChange(hitStopElapsedTime);

	// 半径変更更新処理
	UpdateRadiusChange(hitStopElapsedTime);

	// 高さ変更更新処理
	UpdateHeightChange(hitStopElapsedTime);

	// 幅変更更新処理
	UpdateWidthChange(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	// ムービー時間更新処理
	UpdateMovieTimer(elapsedTime);

	// 無敵時間更新処理
	UpdateInvincibleTimer(elapsedTime);

	// 防御力アップ時間更新
	UpdateDefensesUpTimer(elapsedTime);
}

//! 行列更新処理
void GameObjectBase::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	// ３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

// 行列更新処理(ステージ)
void GameObjectBase::UpdateTransform_Stage()
{
	// 以前の変換行列を保存
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

//! 行列更新処理(飛び道具)
void GameObjectBase::UpdateTransform_Projectile()
{
	DirectX::XMVECTOR Front, Up, Right;

	// 前ベクトルを算出
	Front = DirectX::XMLoadFloat3(&direction);


	// 仮の上ベクトルを算出
	Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	// 右ベクトルを算出
	Right = DirectX::XMVector3Cross(Up, Front);
	Right = DirectX::XMVector3Normalize(Right);


	// 上ベクトルを算出
	Up = DirectX::XMVector3Cross(Front, Right);
	Up = DirectX::XMVector3Normalize(Up);


	// 計算結果を取り出し
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	// 算出した軸ベクトルから行列を作成
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0.0f;
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0.0f;
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0.0f;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

	// 発射方向
	this->direction = front;
}

//! レイキャスト更新処理
void GameObjectBase::UpdateProjectileRaycast()
{
	/*! 地面とのレイキャスト */
#if 1
	{
		// 飛び道具のY軸方向となる法線ベクトル
		DirectX::XMFLOAT3 normal = { 0,1,0 };

		float collisionOffsetY = collisionOffset.y - (radius * 0.5f);

		// レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x + collisionOffset.x, (position.y + 1.0f) + collisionOffsetY, position.z + collisionOffset.z };
		// レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x + collisionOffset.x, (position.y) + collisionOffsetY, position.z + collisionOffset.z };

		// レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 法線ベクトル取得
			normal = hit.normal;

			isGround = true;
		}
		else
			isGround = false;
	}
#endif

	/*! 壁とのレイキャスト */
#if 1
	{
		// レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = {
			position.x + collisionOffset.x,
			position.y + collisionOffset.y + 0.01f,
			position.z + collisionOffset.z };

		//! 当たり判定
		float collisionX = this->radius * (direction.x > 0 ? 1 : -1);;
		float collisionZ = this->radius * (direction.z > 0 ? 1 : -1);;

		DirectX::XMFLOAT3 end = {
				(position.x + collisionOffset.x) + collisionX,
				(position.y + collisionOffset.y + 0.01f),
				(position.z + collisionOffset.z) + collisionZ
		};

		// レイキャストによる壁判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//! 壁と接触した
			isWall = true;
		}
		else
		{
			//! 壁と接触していない
			isWall = false;
		}
	}
#endif
}

//! ヒットストップ更新処理
bool GameObjectBase::UpdateHitStop(float elapsedTime)
{
	//! ヒットストップじゃないなら、処理を止める
	if (!hitStop)
	{
		//! 通常時はelapsedTimeに設定
		hitStopElapsedTime = elapsedTime;

		//! ヒットストップタイムを設定
		hitStopSeconds = hitStopMaxSeconds;

		return false;
	}

	//! ヒットストップの時間が0より大きいなら、時間を減らす
	if (hitStopSeconds > 0)
		hitStopSeconds -= elapsedTime;
	else
	{
		hitStopSeconds = 0;         // 0にする
		hitStop = false;     // ヒットストップを解除
	}

	//! ヒットストップ込みの時間を設定
	hitStopElapsedTime = (1.0f - (hitStopSeconds / hitStopMaxSeconds)) * elapsedTime;

	//! ヒットストップ中にカメラを揺らすなら
	if (hitStopCameraShake)
		//! カメラを少し揺らす
		SetCameraShake(0.05f, DirectX::XMINT3(5, 8, 5));

	return true;
}

/********************************************************************************************/

/*! 速力処理 */
#if 1

//! 加速度更新処理
void GameObjectBase::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	// 垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);

	// 水平速力更新処理
	UpdateHorizontalVelocity(elapsedFrame);

	// 垂直移動更新処理
	UpdateVerticalMove(elapsedTime);

	// 水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
}

// 垂直速力更新処理
void GameObjectBase::UpdateVerticalVelocity(float elapsedFrame)
{
	// TODO:重力処理
	velocity.y += -gravity * elapsedFrame;
}

// 垂直移動更新処理
void GameObjectBase::UpdateVerticalMove(float elapsedTime)
{
	// 垂直方向の移動量
	float my = velocity.y * elapsedTime;

	// キャラクターのY軸方向となる法線ベクトル
	DirectX::XMFLOAT3 normal = { 0,1,0 };

	// 落下中
	if (my < 0.0f)
	{
		//! 当たり判定の位置Y
		float collisionOffsetY = 0;
		switch (collisionType)
		{
			//! ボックス
		case CollisionType::Box:
			collisionOffsetY = collisionOffset.y - (height * 0.5f);
			break;
			//! 球
		case CollisionType::Sphere:
			collisionOffsetY = collisionOffset.y - (radius * 0.82f);
			break;
			//! 円柱
		case CollisionType::Cylinder:
			collisionOffsetY = collisionOffset.y;
			break;
		default:
			break;
		}

		// レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x + collisionOffset.x, (position.y + stepOffset) + collisionOffsetY, position.z + collisionOffset.z };
		// レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x + collisionOffset.x, (position.y + my) + collisionOffsetY, position.z + collisionOffset.z };

		// レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 法線ベクトル取得
			normal = hit.normal;

			// 地面に接地している
			position.x = hit.position.x - collisionOffset.x;
			position.y = hit.position.y - collisionOffsetY;
			position.z = hit.position.z - collisionOffset.z;

			// 着地した
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			// 空中に浮いている
			position.y += my;
			isGround = false;
		}
	}
	// 上昇中
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
}

// 水平速力更新処理
void GameObjectBase::UpdateHorizontalVelocity(float elapsedFrame)
{
	/// XZ平面の速力を減速する
	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMFLOAT3 velocityXZ = { velocity.x, 0.0f, velocity.z };  /// XZ平面の速力
	DirectX::XMVECTOR velocityXZVec = DirectX::XMLoadFloat3(&velocityXZ);

	/// XZ平面の速力
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(velocityXZVec));

	/// XZ平面の速力を加速する
	{
		// 移動ベクトルがゼロベクトルでないなら加速する
		float moveVecLength = sqrt(moveVecX * moveVecX + moveVecZ * moveVecZ);

		// 摩擦による横方向の減速処理
		if (moveVecLength > 0.0f)
		{
			// TODO:キャラクターの加速力
			float acceleration = this->acceleration * elapsedFrame;

			DirectX::XMFLOAT3 moveV = DirectX::XMFLOAT3(moveVecX, 0, moveVecZ);
			DirectX::XMVECTOR moveXZVec = DirectX::XMLoadFloat3(&moveV);

			// 移動ベクトルによる加速処理
			DirectX::XMVECTOR moveVecNormalized = DirectX::XMVector3Normalize(moveXZVec);
			DirectX::XMVECTOR accelerationVec = DirectX::XMVectorScale(moveVecNormalized, acceleration);
			velocityXZVec = DirectX::XMVectorAdd(velocityXZVec, accelerationVec);

			// 最大速力制限
			float newLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(velocityXZVec));
			if (newLength > maxMoveSpeed)
			{
				velocityXZVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(velocityXZVec), maxMoveSpeed);
			}
		}

		else
		{
			// 移動ベクトルがゼロの場合は摩擦による減速のみ行う
			DirectX::XMVECTOR frictionVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(velocityXZVec), -friction * elapsedFrame);

			// 摩擦で逆方向に速度がつかないようにする
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(velocityXZVec)) <
				DirectX::XMVectorGetX(DirectX::XMVector3Length(frictionVec)))
			{
				velocityXZVec = DirectX::XMVectorZero();
			}
			else
			{
				velocityXZVec = DirectX::XMVectorAdd(velocityXZVec, frictionVec);
			}
		}
	}

	// 更新されたXZ速度を保存
	DirectX::XMStoreFloat3(&velocityXZ, velocityXZVec);
	velocity.x = velocityXZ.x;
	velocity.z = velocityXZ.z;

	// 移動ベクトルをリセット
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

// 水平移動更新処理
void GameObjectBase::UpdateHorizontalMove(float elapsedTime)
{
	// 水平速力量計算
	float velocityLengthXZ = sqrt((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (velocityLengthXZ > 0.0f)
	{
		// TODO:水平移動値
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = {
			position.x + collisionOffset.x,
			position.y + collisionOffset.y + 0.01f,
			position.z + collisionOffset.z };

		//! 当たり判定
		float collisionX = 0;
		float collisionZ = 0;

		//! 当たり判定の種類によって変える
		switch (collisionType)
		{
		case CollisionType::Box:
		{
			collisionX = (this->width * 0.5f) * (mx > 0 ? 1 : -1);
			collisionZ = (this->depth * 0.5f) * (mz > 0 ? 1 : -1);
		}
		break;
		case CollisionType::Sphere:
		case CollisionType::Cylinder:
		{
			collisionX = this->radius * (mx > 0 ? 1 : -1);
			collisionZ = this->radius * (mz > 0 ? 1 : -1);
		}
		break;
		default:
			break;
		}

		DirectX::XMFLOAT3 end = {
			(position.x + collisionOffset.x) + collisionX,
			(position.y + collisionOffset.y + 0.01f),
			(position.z + collisionOffset.z) + collisionZ
		};

		// レイキャストによる壁判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//! 加速度を0にする
			velocity.x = 0;
			velocity.z = 0;

			//! 壁と接触した
			isWall = true;
		}
		else
		{
			//! 移動
			position.x += mx;
			position.z += mz;

			//! 壁と接触していない
			isWall = false;
		}
	}
}

#endif // 速力処理

/********************************************************************************************/

/*! 位置変更更新処理 */
#if 1

//! 位置全体変更更新処理
bool GameObjectBase::UpdatePositionAllChange(float elapsedTime)
{
	//! 位置を変えないなら、処理を止める
	if (!positionChange.all)
		return false;


	//! 経過時間を計測
	positionChangeElapsedTime.all += elapsedTime;

	//! イージングタイム
	float t = positionChangeElapsedTime.all / positionChangeTime.all;

#if 1
	switch (positionChangeEasing.all)
	{
		//! リニア(補完無し)
	case PositionChangeEasing::Type::Linear:
	{
		//! 位置変更
		position.x = Easing::Linear(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::Linear(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::Linear(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! イーズイン(加速)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! 位置変更
		position.x = Easing::EaseIn(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseIn(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseIn(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! イーズアウト(減速)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! 位置変更
		position.x = Easing::EaseOut(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseOut(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! 位置変更
		position.x = Easing::EaseInOut(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseInOut(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseInOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! 位置変更
		position.x = Easing::EaseBack(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseBack(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseBack(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! 位置変更
		position.x = Easing::EaseBounce(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseBounce(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseBounce(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! 位置変更
		position.x = Easing::EaseElastic(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseElastic(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseElastic(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! 位置変更
		position.x = Easing::EaseInCircle(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseInCircle(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseInCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! 位置変更
		position.x = Easing::EaseOutCircle(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseOutCircle(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! 位置変更
		position.x = Easing::EaseInOutCircle(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseInOutCircle(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseInOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		position = endPositionChange;
		positionChange.all = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------//

//! 位置X変更更新処理
bool GameObjectBase::UpdatePositionXChange(float elapsedTime)
{
	//! 位置を変えないなら、処理を止める
	if (!positionChange.x)
		return false;


	//! 経過時間を計測
	positionChangeElapsedTime.x += elapsedTime;

	//! イージングタイム
	float t = positionChangeElapsedTime.x / positionChangeTime.x;

#if 1
	switch (positionChangeEasing.x)
	{
		//! リニア(補完無し)
	case PositionChangeEasing::Type::Linear:
	{
		//! 位置変更
		position.x = Easing::Linear(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	//! イーズイン(加速)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! 位置変更
		position.x = Easing::EaseIn(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	//! イーズアウト(減速)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! 位置変更
		position.x = Easing::EaseOut(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! 位置変更
		position.x = Easing::EaseInOut(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! 位置変更
		position.x = Easing::EaseBack(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! 位置変更
		position.x = Easing::EaseBounce(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! 位置変更
		position.x = Easing::EaseElastic(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! 位置変更
		position.x = Easing::EaseInCircle(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! 位置変更
		position.x = Easing::EaseOutCircle(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! 位置変更
		position.x = Easing::EaseInOutCircle(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		position.x = endPositionChange.x;
		positionChange.x = false;
	}

	return true;
}

//! 位置Y変更更新処理
bool GameObjectBase::UpdatePositionYChange(float elapsedTime)
{
	//! 位置を変えないなら、処理を止める
	if (!positionChange.y)
		return false;


	//! 経過時間を計測
	positionChangeElapsedTime.y += elapsedTime;

	//! イージングタイム
	float t = positionChangeElapsedTime.y / positionChangeTime.y;

#if 1
	switch (positionChangeEasing.y)
	{
		//! リニア(補完無し)
	case PositionChangeEasing::Type::Linear:
	{
		//! 位置変更
		position.y = Easing::Linear(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	//! イーズイン(加速)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! 位置変更
		position.y = Easing::EaseIn(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	//! イーズアウト(減速)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! 位置変更
		position.y = Easing::EaseOut(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! 位置変更
		position.y = Easing::EaseInOut(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! 位置変更
		position.y = Easing::EaseBack(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! 位置変更
		position.y = Easing::EaseBounce(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! 位置変更
		position.y = Easing::EaseElastic(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! 位置変更
		position.y = Easing::EaseInCircle(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! 位置変更
		position.y = Easing::EaseOutCircle(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! 位置変更
		position.y = Easing::EaseInOutCircle(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		position.y = endPositionChange.y;
		positionChange.y = false;
	}

	return true;
}

//! 位置Z変更更新処理
bool GameObjectBase::UpdatePositionZChange(float elapsedTime)
{
	//! 位置を変えないなら、処理を止める
	if (!positionChange.z)
		return false;


	//! 経過時間を計測
	positionChangeElapsedTime.z += elapsedTime;

	//! イージングタイム
	float t = positionChangeElapsedTime.z / positionChangeTime.z;

#if 1
	switch (positionChangeEasing.z)
	{
		//! リニア(補完無し)
	case PositionChangeEasing::Type::Linear:
	{
		//! 位置変更
		position.z = Easing::Linear(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! イーズイン(加速)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! 位置変更
		position.z = Easing::EaseIn(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! イーズアウト(減速)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! 位置変更
		position.z = Easing::EaseOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! 位置変更
		position.z = Easing::EaseInOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! 位置変更
		position.z = Easing::EaseBack(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! 位置変更
		position.z = Easing::EaseBounce(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! 位置変更
		position.z = Easing::EaseElastic(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! 位置変更
		position.z = Easing::EaseInCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! 位置変更
		position.z = Easing::EaseOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! 位置変更
		position.z = Easing::EaseInOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		position.z = endPositionChange.z;
		positionChange.z = false;
	}

	return true;
}

#endif // 位置変更更新処理

/********************************************************************************************/

//! 角度制限更新処理
void GameObjectBase::LimitAngle()
{
	/// X軸の回転値を-3.14～3.14に収める
	if (angle.x < -DirectX::XM_PI) angle.x += DirectX::XM_2PI;
	if (angle.x > DirectX::XM_PI)  angle.x -= DirectX::XM_2PI;

	// Y軸の回転値を-3.14～3.14に収める
	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI)  angle.y -= DirectX::XM_2PI;

	// Z軸の回転値を-3.14～3.14に収める
	if (angle.z < -DirectX::XM_PI) angle.z += DirectX::XM_2PI;
	if (angle.z > DirectX::XM_PI)  angle.z -= DirectX::XM_2PI;
}

//------------------------------------------------------------------------------------------//

/*! 角度変更更新処理 */
#if 1

//! 角度全体変更更新処理
bool GameObjectBase::UpdateAngleChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange.all)
		return false;


	//! 経過時間を計測
	angleChangeElapsedTime.all += elapsedTime;

	//! イージングタイム
	float t = angleChangeElapsedTime.all / angleChangeTime.all;

#if 1
	switch (angleChangeEasing.all)
	{
		//! リニア(補完無し)
	case AngleChangeEasing::Type::Linear:
	{
		//! 角度変更
		angle.x = Easing::Linear(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::Linear(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::Linear(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! イーズイン(加速)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! 角度変更
		angle.x = Easing::EaseIn(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseIn(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseIn(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! イーズアウト(減速)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! 角度変更
		angle.x = Easing::EaseOut(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseOut(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! 角度変更
		angle.x = Easing::EaseInOut(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseInOut(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseInOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! 角度変更
		angle.x = Easing::EaseBack(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseBack(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseBack(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! 角度変更
		angle.x = Easing::EaseBounce(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseBounce(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseBounce(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! 角度変更
		angle.x = Easing::EaseElastic(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseElastic(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseElastic(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! 角度変更
		angle.x = Easing::EaseInCircle(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseInCircle(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseInCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! 角度変更
		angle.x = Easing::EaseOutCircle(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseOutCircle(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! 角度変更
		angle.x = Easing::EaseInOutCircle(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseInOutCircle(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseInOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		angle = endAngleChange;
		angleChange.all = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------//

//! 角度X変更更新処理
bool GameObjectBase::UpdateAngleXChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange.x)
		return false;


	//! 経過時間を計測
	angleChangeElapsedTime.x += elapsedTime;

	//! イージングタイム
	float t = angleChangeElapsedTime.x / angleChangeTime.x;

#if 1
	switch (angleChangeEasing.x)
	{
		//! リニア(補完無し)
	case AngleChangeEasing::Type::Linear:
	{
		//! 角度変更
		angle.x = Easing::Linear(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	//! イーズイン(加速)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! 角度変更
		angle.x = Easing::EaseIn(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	//! イーズアウト(減速)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! 角度変更
		angle.x = Easing::EaseOut(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! 角度変更
		angle.x = Easing::EaseInOut(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! 角度変更
		angle.x = Easing::EaseBack(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! 角度変更
		angle.x = Easing::EaseBounce(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! 角度変更
		angle.x = Easing::EaseElastic(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! 角度変更
		angle.x = Easing::EaseInCircle(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! 角度変更
		angle.x = Easing::EaseOutCircle(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! 角度変更
		angle.x = Easing::EaseInOutCircle(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		angle.x = endAngleChange.x;
		angleChange.x = false;
	}

	return true;
}

//! 角度Y変更更新処理
bool GameObjectBase::UpdateAngleYChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange.y)
		return false;


	//! 経過時間を計測
	angleChangeElapsedTime.y += elapsedTime;

	//! イージングタイム
	float t = angleChangeElapsedTime.y / angleChangeTime.y;

#if 1
	switch (angleChangeEasing.y)
	{
		//! リニア(補完無し)
	case AngleChangeEasing::Type::Linear:
	{
		//! 角度変更
		angle.y = Easing::Linear(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	//! イーズイン(加速)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! 角度変更
		angle.y = Easing::EaseIn(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	//! イーズアウト(減速)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! 角度変更
		angle.y = Easing::EaseOut(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! 角度変更
		angle.y = Easing::EaseInOut(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! 角度変更
		angle.y = Easing::EaseBack(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! 角度変更
		angle.y = Easing::EaseBounce(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! 角度変更
		angle.y = Easing::EaseElastic(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! 角度変更
		angle.y = Easing::EaseInCircle(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! 角度変更
		angle.y = Easing::EaseOutCircle(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! 角度変更
		angle.y = Easing::EaseInOutCircle(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		angle.y = endAngleChange.y;
		angleChange.y = false;
	}

	return true;
}

//! 角度Z変更更新処理
bool GameObjectBase::UpdateAngleZChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange.z)
		return false;


	//! 経過時間を計測
	angleChangeElapsedTime.z += elapsedTime;

	//! イージングタイム
	float t = angleChangeElapsedTime.z / angleChangeTime.z;

#if 1
	switch (angleChangeEasing.z)
	{
		//! リニア(補完無し)
	case AngleChangeEasing::Type::Linear:
	{
		//! 角度変更
		angle.z = Easing::Linear(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! イーズイン(加速)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! 角度変更
		angle.z = Easing::EaseIn(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! イーズアウト(減速)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! 角度変更
		angle.z = Easing::EaseOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! 角度変更
		angle.z = Easing::EaseInOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! 角度変更
		angle.z = Easing::EaseBack(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! 角度変更
		angle.z = Easing::EaseBounce(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! 角度変更
		angle.z = Easing::EaseElastic(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! 角度変更
		angle.z = Easing::EaseInCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! 角度変更
		angle.z = Easing::EaseOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! 角度変更
		angle.z = Easing::EaseInOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		angle.z = endAngleChange.z;
		angleChange.z = false;
	}

	return true;
}

//! 角度の回転の更新処理(特定の値は目指さず、指定された時間中に回転する)
bool GameObjectBase::UpdateAngleRotation(float elapsedTime)
{
	//! 角度を回転しないなら、処理を止める
	if (!angleRotation)
		return false;

	//! 回転時間が0より大きいなら
	if (rotationTime > 0.0f)
		// 回転時間を減らす
		rotationTime -= elapsedTime;
	else
		angleRotation = false;

	//! 回転処理
	angle.x += angleRotationSpeed.x * elapsedTime;
	angle.y += angleRotationSpeed.y * elapsedTime;
	angle.z += angleRotationSpeed.z * elapsedTime;

	return true;
}

#endif // 角度変更更新処理

/********************************************************************************************/

/*! スケール変更更新処理 */
#if 1
//! スケール全体変更更新処理
bool GameObjectBase::UpdateScaleChange(float elapsedTime)
{
	//! 大きさを変更しないなら、処理を止める
	if (!scaleChange.all)
		return false;


	//! 経過時間を計測
	scaleChangeElapsedTime.all += elapsedTime;

	//! イージングタイム
	float t = scaleChangeElapsedTime.all / scaleChangeTime.all;

#if 1
	switch (scaleChangeEasing.all)
	{
		//! リニア(補完無し)
	case ScaleChangeEasing::Type::Linear:
	{
		//! 大きさ変更
		scale.x = Easing::Linear(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::Linear(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::Linear(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! イーズイン(加速)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! 大きさ変更
		scale.x = Easing::EaseIn(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseIn(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseIn(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! イーズアウト(減速)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! 大きさ変更
		scale.x = Easing::EaseOut(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseOut(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! 大きさ変更
		scale.x = Easing::EaseInOut(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseInOut(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseInOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! 大きさ変更
		scale.x = Easing::EaseBack(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseBack(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseBack(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! 大きさ変更
		scale.x = Easing::EaseBounce(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseBounce(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseBounce(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! 大きさ変更
		scale.x = Easing::EaseElastic(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseElastic(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseElastic(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! 大きさ変更
		scale.x = Easing::EaseInCircle(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseInCircle(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseInCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! 大きさ変更
		scale.x = Easing::EaseOutCircle(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseOutCircle(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! 大きさ変更
		scale.x = Easing::EaseInOutCircle(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseInOutCircle(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseInOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		scale = endScaleChange;
		scaleChange.all = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------//

//! スケールX変更更新処理
bool GameObjectBase::UpdateScaleXChange(float elapsedTime)
{
	//! スケールを変えないなら、処理を止める
	if (!scaleChange.x)
		return false;


	//! 経過時間を計測
	scaleChangeElapsedTime.x += elapsedTime;

	//! イージングタイム
	float t = scaleChangeElapsedTime.x / scaleChangeTime.x;

#if 1
	switch (scaleChangeEasing.x)
	{
		//! リニア(補完無し)
	case ScaleChangeEasing::Type::Linear:
	{
		//! スケール変更
		scale.x = Easing::Linear(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	//! イーズイン(加速)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! スケール変更
		scale.x = Easing::EaseIn(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	//! イーズアウト(減速)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! スケール変更
		scale.x = Easing::EaseOut(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! スケール変更
		scale.x = Easing::EaseInOut(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! スケール変更
		scale.x = Easing::EaseBack(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! スケール変更
		scale.x = Easing::EaseBounce(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! スケール変更
		scale.x = Easing::EaseElastic(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! スケール変更
		scale.x = Easing::EaseInCircle(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! スケール変更
		scale.x = Easing::EaseOutCircle(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! スケール変更
		scale.x = Easing::EaseInOutCircle(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		scale.x = endScaleChange.x;
		scaleChange.x = false;
	}

	return true;
}

//! スケールY変更更新処理
bool GameObjectBase::UpdateScaleYChange(float elapsedTime)
{
	//! スケールを変えないなら、処理を止める
	if (!scaleChange.y)
		return false;


	//! 経過時間を計測
	scaleChangeElapsedTime.y += elapsedTime;

	//! イージングタイム
	float t = scaleChangeElapsedTime.y / scaleChangeTime.y;

#if 1
	switch (scaleChangeEasing.y)
	{
		//! リニア(補完無し)
	case ScaleChangeEasing::Type::Linear:
	{
		//! スケール変更
		scale.y = Easing::Linear(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	//! イーズイン(加速)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! スケール変更
		scale.y = Easing::EaseIn(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	//! イーズアウト(減速)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! スケール変更
		scale.y = Easing::EaseOut(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! スケール変更
		scale.y = Easing::EaseInOut(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! スケール変更
		scale.y = Easing::EaseBack(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! スケール変更
		scale.y = Easing::EaseBounce(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! スケール変更
		scale.y = Easing::EaseElastic(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! スケール変更
		scale.y = Easing::EaseInCircle(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! スケール変更
		scale.y = Easing::EaseOutCircle(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! スケール変更
		scale.y = Easing::EaseInOutCircle(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		scale.y = endScaleChange.y;
		scaleChange.y = false;
	}

	return true;
}

//! スケールZ変更更新処理
bool GameObjectBase::UpdateScaleZChange(float elapsedTime)
{
	//! スケールを変えないなら、処理を止める
	if (!scaleChange.z)
		return false;


	//! 経過時間を計測
	scaleChangeElapsedTime.z += elapsedTime;

	//! イージングタイム
	float t = scaleChangeElapsedTime.z / scaleChangeTime.z;

#if 1
	switch (scaleChangeEasing.z)
	{
		//! リニア(補完無し)
	case ScaleChangeEasing::Type::Linear:
	{
		//! スケール変更
		scale.z = Easing::Linear(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! イーズイン(加速)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! スケール変更
		scale.z = Easing::EaseIn(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! イーズアウト(減速)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! スケール変更
		scale.z = Easing::EaseOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! スケール変更
		scale.z = Easing::EaseInOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! スケール変更
		scale.z = Easing::EaseBack(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! スケール変更
		scale.z = Easing::EaseBounce(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! スケール変更
		scale.z = Easing::EaseElastic(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! スケール変更
		scale.z = Easing::EaseInCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! スケール変更
		scale.z = Easing::EaseOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! スケール変更
		scale.z = Easing::EaseInOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		scale.z = endScaleChange.z;
		scaleChange.z = false;
	}

	return true;
}

#endif // スケール変更更新処理

//------------------------------------------------------------------------------------------//

/*! 当たり判定*/
#if 1

//! 当たり判定の位置を設定
void GameObjectBase::CollisionPosSettings()
{
	collisionPos = {
		position.x + collisionOffset.x,
		position.y + collisionOffset.y,
		position.z + collisionOffset.z
	};
}

//! 当たり判定のオフセット変更更新処理
bool GameObjectBase::UpdateCollisionOffsetChange(float elapsedTime)
{
	//! 当たり判定のオフセットを変えないなら、処理を止める
	if (!collisionOffsetChange)
		return false;


	//! 経過時間を計測
	collisionOffsetChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = collisionOffsetChangeElapsedTime / collisionOffsetChangeTime;

	//! 当たり判定のオフセットを変更
	collisionOffset.x = Easing::Linear(collisionOffset.x, endCollisionOffsetChange.x, t);
	collisionOffset.y = Easing::Linear(collisionOffset.y, endCollisionOffsetChange.y, t);
	collisionOffset.z = Easing::Linear(collisionOffset.z, endCollisionOffsetChange.z, t);

	//! 当たり判定のオフセット変更が終わったら、処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		collisionOffset = endCollisionOffsetChange;
		collisionOffsetChange = false;
	}

	return true;
}

//! 半径変更更新処理
bool GameObjectBase::UpdateRadiusChange(float elapsedTime)
{
	//! 半径を変更しないなら、処理を止める
	if (!radiusChange)
		return false;


	//! 経過時間を計測
	radiusChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = radiusChangeElapsedTime / radiusChangeTime;

	//! 半径を変更
	radius = Easing::Linear(startRadiusChange, endRadiusChange, t);

	//! 半径の変更が終わったら
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		radius = endRadiusChange;
		radiusChange = false;
	}

	return true;
}

//! 高さ変更更新処理
bool GameObjectBase::UpdateHeightChange(float elapsedTime)
{
	//! 高さを変更しないなら、処理を止める
	if (!heightChange)
		return false;


	//! 経過時間を計測
	heightChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = heightChangeElapsedTime / heightChangeTime;

	//! 高さを変更
	height = Easing::Linear(startHeightChange, endHeightChange, t);

	//! 高さの変更が終わったら
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		height = endHeightChange;
		heightChange = false;
	}

	return true;
}

//! 幅変更更新処理
bool GameObjectBase::UpdateWidthChange(float elapsedTime)
{
	//! 幅を変更しないなら、処理を止める
	if (!widthChange)
		return false;


	//! 経過時間を計測
	widthChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = widthChangeElapsedTime / widthChangeTime;

	//! 幅を変更
	width = Easing::Linear(startWidthChange, endWidthChange, t);

	//! 幅の変更が終わったら
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		width = endWidthChange;
		widthChange = false;
	}

	return true;
}

#endif // 当たり判定

/********************************************************************************************/

/*! 描画関連 */
#if 1

//! マテリアルの色変更更新処理
bool GameObjectBase::UpdateMaterialColorChange(float elapsedTime)
{
	//! マテリアルの色を変えないなら、処理を止める
	if (!materialColorChange)
		return false;


	//! 経過時間を計測
	materialColorChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = materialColorChangeElapsedTime / materialColorChangeTime;

	//! マテリアルの色を変更
	materialColor.x = Easing::Linear(materialColor.x, endMaterialColorChange.x, t);
	materialColor.y = Easing::Linear(materialColor.y, endMaterialColorChange.y, t);
	materialColor.z = Easing::Linear(materialColor.z, endMaterialColorChange.z, t);

	//! マテリアルの色変更が終わったら、処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		materialColor = endMaterialColorChange;
		materialColorChange = false;
	}

	return true;
}

//! 不透明度変更更新処理
bool GameObjectBase::UpdateOpacityChange(float elapsedTime)
{
	//! 不透明度を変更しないなら、処理を止める
	if (!opacityChange)
		return false;


	//! 経過時間を計測
	opacityChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = opacityChangeElapsedTime / opacityChangeTime;

	//! 不透明度を変更
	opacity = Easing::Linear(startOpacityChange, endOpacityChange, t);

	//! 不透明度の変更が終わったら
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		opacity = endOpacityChange;
		opacityChange = false;
	}

	return true;
}

//! エミッシブの色変更更新処理
bool GameObjectBase::UpdateEmissiveColorChange(float elapsedTime)
{
	//! エミッシブの色を変えないなら、処理を止める
	if (!emissiveColorChange)
		return false;


	//! 経過時間を計測
	emissiveColorChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = emissiveColorChangeElapsedTime / emissiveColorChangeTime;

	//!エミッシブの色を変更
	emissiveColor.x = Easing::Linear(startEmissiveColorChange.x, endEmissiveColorChange.x, t);
	emissiveColor.y = Easing::Linear(startEmissiveColorChange.y, endEmissiveColorChange.y, t);
	emissiveColor.z = Easing::Linear(startEmissiveColorChange.z, endEmissiveColorChange.z, t);

	//! 処理を止める
	if (t >= 1.0f);
	{
		//! 値のずれを無くす
		emissiveColor = endEmissiveColorChange;
		emissiveColorChange = false;
	}

	return true;
}

//! エミッシブの強さ変更更新処理
bool GameObjectBase::UpdateEmissiveStrengthChange(float elapsedTime)
{
	//! エミッシブの強さを変えないなら、処理を止める
	if (!emissiveStrengthChange)
		return false;


	//! 経過時間を計測
	emissiveStrengthChangeElapsedTime += elapsedTime;

	//! イージングタイム
	float t = emissiveStrengthChangeElapsedTime / emissiveStrengthChangeTime;

	//! エミッシブの強さを変更
	emissiveStrength = Easing::Linear(startEmissiveStrengthChange, endEmissiveStrengthChange, t);

	//! 処理を止める
	if (t >= 1.0f)
	{
		//! 値のずれを無くす
		emissiveStrength = endEmissiveStrengthChange;
		emissiveStrengthChange = false;
	}

	return true;
}

//! エミッシブの形態変化演出更新処理
bool GameObjectBase::UpdateEmissivePhaseChange(float elapsedTime)
{
	//!エミッシブの形態変化演出をしないなら、処理を止める
	if (!emissivePhaseChange)
		return false;


	//! タイマーを減らす
	if (emissivePhaseChangeTimer > 0.0f)
		emissivePhaseChangeTimer -= elapsedTime;
	else
	{
		emissiveColor = { 0, 0, 0 }; //エミッシブカラー
		emissiveStrength = 0.0f;        //エミッシブの強さ

		emissivePhaseChange = false;
	}

	//!エミッシブの色変更更新処理
	UpdateEmissiveColorPhaseChange(emissiveColor.x, emissiveColorPhaseChangeSpeed.x, emissiveColorChangeDown.x, elapsedTime);
	UpdateEmissiveColorPhaseChange(emissiveColor.y, emissiveColorPhaseChangeSpeed.y, emissiveColorChangeDown.y, elapsedTime);
	UpdateEmissiveColorPhaseChange(emissiveColor.z, emissiveColorPhaseChangeSpeed.z, emissiveColorChangeDown.z, elapsedTime);

	return true;
}

// エミッシブの色変更更新処理
void GameObjectBase::UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeTime, bool& emissiveColorDown, float elapsedTime)
{
	//! カラー
	emissiveColor += (emissiveColorDown ? -1 : 1) * emissiveColorChangeTime * elapsedTime;

	//! 色が最小値より小さい、または、最大値より大きいなら
	if (emissiveColor < hpImageColorMin || emissiveColor > hpImageColorMax)
	{
		// 数値を代入
		emissiveColor = std::clamp(emissiveColor,
			emissiveColorMin, emissiveColorMax);

		emissiveColorDown = !emissiveColorDown; // フラグを反転する
	}
}

#endif // 描画関連

//******************************************************************************************//

//! ムービーシーンの待ち時間処理
bool GameObjectBase::UpdateMovieTimer(float elapsedTime)
{
	// ムービーシーンで無いなら、処理を止める
	if (!movieScene)
		return false;

	// 待ち時間が終わったら
	if (movieTime <= 0.0f)
		movieScene = false;

	movieTime -= elapsedTime;

	return true;
}

//! 無敵時間更新
void GameObjectBase::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
		invincibleTimer -= elapsedTime;
}

//! 防御力アップ時間更新
void GameObjectBase::UpdateDefensesUpTimer(float elapsedTime)
{
	if (defensesUpTimer > 0.0f)
	{
		defensesUpTimer -= elapsedTime;
	}
	if (defensesUpTimer <= 0)
	{
		defensesUpTimer = 0; // 防御力アップ時間ではない
		defenses = 0;        // 防御力リセット
	}
}