#pragma once

#include <DirectXMath.h>
#include <memory>
#include <stdexcept>
#include "Camera.h"
#include "Game/Character/Director/DirectorManager.h"

// 前方宣言
class Graphics;
class Fade;

// カメラコントローラー
class CameraController
{
public:
	CameraController();
	~CameraController();

	// 唯一のインスタンス取得
	static CameraController& Instance()
	{
		static CameraController cameraController;
		return cameraController;
	}

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void RenderCameraTarget(ID3D11DeviceContext* dc, Shader* shader);

	// フェードを描画
	void FadeRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// デバッグ描画
	void DrawDebugGUI();

	/************************************************************************************************/
		/*! どのイージングにするか */
#if 1
	//! ターゲット
	enum class TargetChangeEasing
	{
		Linear,  // リニア(補完無し)
		EaseIn,  // イーズイン(加速)
		EaseOut, // イーズアウト(減速)
	};

	static TargetChangeEasing targetChangeEasing;

	//! 角度
	enum class AngleChangeEasing
	{
		Linear,  // リニア(補完無し)
		EaseIn,  // イーズイン(加速)
		EaseOut, // イーズアウト(減速)
	};

	static AngleChangeEasing angleChangeEasing;

	//! 範囲
	enum class RangeChangeEasing
	{
		Linear,  // リニア(補完無し)
		EaseIn,  // イーズイン(加速)
		EaseOut, // イーズアウト(減速)
	};

	static RangeChangeEasing rangeChangeEasing;

#endif

	/************************************************************************************************/
		/*! セッター */

#if 1
	/*! ターゲット */
#if 1

	// ターゲットの位置設定(デバッグカメラでは作動しない)
	void SetTarget(const DirectX::XMFLOAT3& target)
	{
		//! デバッグカメラじゃないなら
		if (!debugCamera)
			this->target = target;
	}

	// ターゲットの位置変更の設定
	void SetTargetChange(DirectX::XMFLOAT3 endTargetChange, float targetChangeTime, TargetChangeEasing targetChangeEasing = TargetChangeEasing::Linear)
	{
		if (!targetChange &&
			(target.x != endTargetChange.x
				|| target.y != endTargetChange.y
				|| target.z != endTargetChange.z))
		{
			targetChange = true;               // ターゲットの位置を変える
			this->startTargetChange = this->target;       // ターゲットの変更の開始の値
			this->endTargetChange = endTargetChange;    // ここまでターゲットの位置を変える
			this->targetChangeTime = targetChangeTime;   // ターゲットの位置を変える時間
			this->targetChangeEasing = targetChangeEasing; // どのイージングにするか
			this->targetChangeElapsedTime = 0.0f;               // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->targetChangeTime <= 0)
			{
				throw std::out_of_range("イージング出来ません！！");
			}
		}
	}

	// ターゲットのリセット
	void SetTargetReset() { this->target = this->targetReset; }

	// ターゲットのリセットの設定
	void SetTargetResetChange(float targetResetChangeTime = 0.5f, TargetChangeEasing targetChangeEasing = TargetChangeEasing::Linear)
	{
		//! カメラの位置の変更を解除
		targetChange = false;

		if ((target.x != targetReset.x
			|| target.y != targetReset.y
			|| target.z != targetReset.z))
		{
			this->targetChange = true;                  // カメラのターゲットの位置を変える
			this->startTargetChange = target;                // カメラのターゲットの変更の開始の値
			this->endTargetChange = targetReset;           // ここまでターゲットの位置を変える
			this->targetChangeTime = targetResetChangeTime; // ターゲットの位置を変える時間
			this->targetChangeEasing = targetChangeEasing;    // どのイージングにするか
			this->targetChangeElapsedTime = 0.0f;                  // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (targetChangeTime <= 0)
			{
				throw std::out_of_range("イージング出来ません！！");
			}
		}
	}

#endif

	/*! 角度 */
#if 1

	// 角度の設定(デバッグカメラでは作動しない)
	void SetAngle(const DirectX::XMFLOAT3& angle)
	{
		//! デバッグカメラじゃないなら
		if (!debugCamera)
			this->angle = angle;
	}

	// 角度変更の設定
	void SetAngleChange(DirectX::XMFLOAT3 endAngleChange, float angleChangeTime, AngleChangeEasing angleChangeEasing = AngleChangeEasing::Linear)
	{
		if (!angleChange &&
			(angle.x != endAngleChange.x
				|| angle.y != endAngleChange.y
				|| angle.z != endAngleChange.z))
		{
			angleChange = true;              // 角度を変える
			this->startAngleChange = this->angle;       // 角度の変更の開始の値
			this->endAngleChange = endAngleChange;    // ここまで角度を変える
			this->angleChangeTime = angleChangeTime;   // 角度を変える時間
			this->angleChangeEasing = angleChangeEasing; // どのイージングにするか
			this->angleChangeElapsedTime = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->angleChangeTime <= 0)
			{
				throw std::out_of_range("イージング出来ません！！");
			}
		}
	}

	// 角度のリセット
	void SetAngleReset() { this->angle = this->angleReset; }

	// 角度のリセットの設定
	void SetAngleResetChange(float angleResetChangeTime = 0.5f, AngleChangeEasing angleChangeEasing = AngleChangeEasing::Linear)
	{
		//! カメラの角度の変更を解除
		angleChange = false;

		if ((angle.x != angleReset.x)
			|| angle.y != angleReset.y
			|| angle.z != angleReset.z)
		{
			this->angleChange = true;                 // カメラの角度を変える
			this->startAngleChange = angle;				   // カメラの角度の変更の開始の値
			this->endAngleChange = angleReset;           // ここまで角度を変える
			this->angleChangeTime = angleResetChangeTime; // 角度を変える時間
			this->angleChangeEasing = angleChangeEasing;    // どのイージングにするか
			this->angleChangeElapsedTime = 0.0f;                 // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (angleChangeTime <= 0)
			{
				throw std::out_of_range("イージング出来ません！！");
			}
		}
	}

#endif

	/*! 範囲 */
#if 1

	// カメラの範囲の設定(デバッグカメラでは作動しない)
	void SetRange(float range)
	{
		//! デバッグカメラじゃないなら
		if (!debugCamera)
			this->range = range;
	}

	// カメラの範囲の変更の設定
	void SetRangeChange(float endRangeChange, float rangeChangeTime, RangeChangeEasing rangeChangeEasing = RangeChangeEasing::Linear)
	{
		if (!rangeChange && range != endRangeChange)
		{
			rangeChange = true;              // カメラの範囲を変える
			this->startRangeChange = this->range;       // カメラの範囲の変更の開始の値
			this->endRangeChange = endRangeChange;    // ここまでカメラの範囲を変える
			this->rangeChangeTime = rangeChangeTime;   // カメラの範囲を変える時間
			this->rangeChangeEasing = rangeChangeEasing; // どのイージングにするか
			this->rangeChangeElapsedTime = 0.0f;              // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (this->rangeChangeTime <= 0)
			{
				throw std::out_of_range("イージング出来ません！！");
			}
		}
	}

	// カメラの範囲のリセット
	void SetRangeReset() { this->range = this->rangeReset; }

	// カメラの範囲のリセットの設定
	void SetRangeResetChange(float rangeResetChangeTime = 0.5f, RangeChangeEasing rangeChangeEasing = RangeChangeEasing::Linear)
	{
		//! カメラの範囲の変更を解除
		rangeChange = false;

		if (CameraController::range != CameraController::rangeReset)
		{
			rangeChange = true;                         // カメラの範囲を変える
			startRangeChange = CameraController::range;      // カメラの範囲の変更の開始の値
			endRangeChange = CameraController::rangeReset; // ここまでカメラの範囲を変える
			rangeChangeTime = rangeResetChangeTime;         // カメラの範囲を変える時間
			rangeChangeEasing = rangeChangeEasing;            // どのイージングにするか
			rangeChangeElapsedTime = 0.0f;                         // 経過時間をリセット

			// 変更時間の設定が0以下の時にエラーを出す
			if (CameraController::rangeChangeTime <= 0)
			{
				throw std::out_of_range("イージング出来ません！！");
			}
		}
	}

#endif

	// カメラの高さの設定
	void SetHeight(float height) { this->height = height; }

	// カメラを注目させるかの設定
	void SetTracking(bool tracking) { this->tracking = tracking; }

	// カメラムービー時間の設定
	void SetCameraMovieTime(float movieTime)
	{
		this->cameraMovie = true;
		this->movieTime = movieTime;
	}

	// カメラシェイク設定
	void SetCameraShake(float shakeTime, DirectX::XMINT3 shakePower)
	{
		// カメラシェイクじゃ無いなら設定する
		this->cameraShake = true;       // カメラシェイクをする
		this->cameraShakeTimer = shakeTime;  // 揺らす時間
		this->cameraShakePower = shakePower; // 揺らす大きさ
	}

	// カメラシェイクを解除
	void SetCameraShakeUnlock() { this->cameraShake = false; }

	// 他の更新処理を止める設定
	void SetBossFinish()
	{
		this->bossFinishSettings = false; // 設定をしたか
		this->bossFinish = true;  // 他の更新処理を止める
		this->bossFinishTimer = 1.2f;  // 他の更新処理を止める時間
	}

#endif

	/************************************************************************************************/
		/*! ゲッター */

		// カメラがムービー中かの取得
	bool GetCameraMovie() { return cameraMovie; }

	// 他の更新処理を止めるかの取得
	bool GetBossFinish() { return bossFinish; }

	/************************************************************************************************/

		/*! その他の更新処理 */

		// カメラの注目更新処理
	void UpdateCameraTargetTracking(float elapsedTime);

	//-------------------------------------------------------------------//

		// カメラの状態更新処理
	void UpdateCameraState(float elapsedTime);

	// カメラの制限処理
	void CameraLimit();

	// デバッグカメラ
	bool UpdateDebugCamera(float elapsedTime);

	//---------------------------------------------------------------------------//

		// カメラのターゲットの変更更新処理
	bool UpdateTargetChange(float elapsedTime);

	//---------------------------------------------------------------------------//

		// カメラの角度の変更更新処理
	bool UpdateAngleChange(float elapsedTime);

	//---------------------------------------------------------------------------//

		// カメラの範囲の変更更新処理
	bool UpdateRangeChange(float elapsedTime);

	// カメラムービー時間の更新処理
	bool UpdateCameraMovieTimer(float elapsedTime);

	// カメラシェイク更新処理
	bool UpdateCameraShake(float elapsedTime);

	// 他の更新処理を止める更新処理
	bool UpdateBossFinish(float elapsedTime);

private:

	float rollSpeed = DirectX::XMConvertToRadians(90);
	float minAngleX = DirectX::XMConvertToRadians(-90);
	float maxAngleX = DirectX::XMConvertToRadians(90);

public:
	static bool  tracking; // カメラをプレイヤー中心でターゲットに注目させるか
	static float height;   // カメラの高さ
	static float dist;     // プレイヤーとターゲットの距離

	/**********************************************************************************************/
		/*! ターゲット */

public:
	static DirectX::XMFLOAT3 target;      // ターゲット
	static DirectX::XMFLOAT3 targetReset; // ターゲットのリセット位置

	static bool              targetChange;            // ターゲットの位置を変えるか
	static DirectX::XMFLOAT3 startTargetChange;       // ターゲットの変更の開始の値
	static DirectX::XMFLOAT3 endTargetChange;         // ここまでターゲットの位置を変える
	static float             targetChangeTime;        // ターゲットの位置を変える時間
	static float             targetChangeElapsedTime; // ターゲットの位置変更の経過時間

	/**********************************************************************************************/
		/*! 角度 */

public:
	static DirectX::XMFLOAT3 angle;      // 角度
	static DirectX::XMFLOAT3 angleReset; // 角度のリセットの数値

	static bool              angleChange;            // 角度を変えるか
	static DirectX::XMFLOAT3 startAngleChange;       // 角度の変更の開始の値
	static DirectX::XMFLOAT3 endAngleChange;         // ここまで角度を変える
	static float             angleChangeTime;        // 角度を変える時間
	static float             angleChangeElapsedTime; // 角度変更の経過時間

	/**********************************************************************************************/
		/*! カメラの範囲 */

private:
	float minRange = 2.0f;   // カメラ範囲の最小
	float maxRange = 999.0f; // カメラ範囲の最大

public:
	static float range;      // カメラの範囲
	static float rangeReset; // カメラの範囲のリセットの数値

	static bool  rangeChange;            // カメラの範囲を変えるか
	static float startRangeChange;       // カメラの範囲の変更の開始の値
	static float endRangeChange;         // ここまでカメラの範囲を変える
	static float rangeChangeTime;        // カメラの範囲を変える時間
	static float rangeChangeElapsedTime; // カメラの範囲変更の経過時間

	/**********************************************************************************************/
		/*! カメラシェイク */

public:
	static bool  cameraShake;                // カメラを揺らすか
	static float cameraShakeTimer;           // 揺らす時間
	static DirectX::XMINT3 cameraShakePower; // 揺らす大きさ

	/**********************************************************************************************/
public:
	static bool debugCamera; // デバッグカメラ
	static bool noMoveDebugCamera; // デバッグカメラの動きを止めるか
private:
	float mouseMoveSpeed = 1.5f; // マウスの進む速さ
	float mouseZoomSpeed = 0.1f; // マウスの拡大速度
	float mouseSensitivity = 1.0f; // マウス感度

	float targetMoveSpeed = 8; // ターゲットの移動速度(ボタン移動の場合)
	float targetUpSpeed = 8;  // ターゲットの上昇速度(ボタン移動の場合)

	DirectorManager directorManager; // 演出マネージャー

	//-----------------------------------------------------------------//

	bool cameraTargetPlayer_3D = false; // カメラのターゲットをプレイヤーにするか

	bool  cameraMovie = false; // カメラのムービー中か
	float movieTime = 0.0f;  // ムービー時間

	//-----------------------------------------------------------------//

	std::unique_ptr<Fade> fade;
	bool bossFinishSettings = false; // 設定したか

	bool  bossFinish = false; // ボスフィニッシュするか
	float bossFinishTimer = 0.0f;  // ボスフィニッシュ時間
};