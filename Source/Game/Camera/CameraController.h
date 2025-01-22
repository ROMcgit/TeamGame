#pragma once

#include <DirectXMath.h>
#include <memory>
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "Game/Character/Director/SettingsDirector/DirectorManager.h"

// 前方宣言
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
	void RenderTarget(ID3D11DeviceContext* dc, Shader* shader);

	// フェードを描画
	void FadeRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// デバッグ描画
	void DrawDebugGUI();

	/************************************************************************************************/
		/*! セッター */

#if 1
	// ターゲットの位置設定
	void SetTarget(const DirectX::XMFLOAT3& target)
	{
		//! デバッグカメラじゃないなら
		if (!debugCamera)
			this->target = target;
	}

	// ターゲットの位置変更の設定
	void SetTargetChange(DirectX::XMFLOAT3 toTargetChange, DirectX::XMFLOAT3 targetChangeSpeed)
	{
		if (!targetChange)
		{
			targetChange = true;              // ターゲットの位置を変える
			this->toTargetChange = toTargetChange;          // ここまでターゲットの位置を変える
			this->targetChangeSpeed = targetChangeSpeed; // ターゲットの位置を変える速さ

			// ターゲットXが目指す位置より小さい時
			if (target.x < toTargetChange.x)
				targetChangeUp.x = true;
			else
				targetChangeUp.x = false;

			// ターゲットYが目指す位置より小さい時
			if (target.y < toTargetChange.y)
				targetChangeUp.y = true;
			else
				targetChangeUp.y = false;

			// ターゲットZが目指す位置より小さい時
			if (target.z < toTargetChange.z)
				targetChangeUp.z = true;
			else
				targetChangeUp.z = false;
		}
	}

	// 角度設定
	void SetAngle(const DirectX::XMFLOAT3& angle)
	{
		//! デバッグカメラじゃないなら
		if (!debugCamera)
			this->angle = angle;
	}

	// 角度変更の設定
	void SetAngleChange(DirectX::XMFLOAT3 toAngleChange, DirectX::XMFLOAT3 angleChangeSpeed)
	{
		if (!angleChange)
		{
			angleChange = true;             // 角度を変える
			this->toAngleChange = toAngleChange;          // ここまで角度を変える
			this->angleChangeSpeed = angleChangeSpeed; // 角度を変える速さ

			// 角度Xが目指す位置より小さい時
			if (angle.x < toAngleChange.x)
				angleChangeUp.x = true;
			else
				angleChangeUp.x = false;

			// 角度Yが目指す位置より小さい時
			if (angle.y < toAngleChange.y)
				angleChangeUp.y = true;
			else
				angleChangeUp.y = false;

			// 角度Zが目指す位置より小さい時
			if (angle.z < toAngleChange.z)
				angleChangeUp.z = true;
			else
				angleChangeUp.z = false;
		}
	}

	// カメラの範囲の設定
	void SetRange(float range)
	{
		//! デバッグカメラじゃないなら
		if (!debugCamera)
			this->range = range;
	}

	// カメラの範囲の変更の設定
	void SetRangeChange(float toRangeChange, float rangeChangeSpeed)
	{
		if (!rangeChange)
		{
			rangeChange = true;             // カメラの範囲を変える
			this->toRangeChange = toRangeChange;          // ここまでカメラの範囲を変える
			this->rangeChangeSpeed = rangeChangeSpeed; // カメラの範囲を変える速さ

			// カメラの範囲が目指す大きさより小さい時
			if (range < toRangeChange)
				rangeChangeUp = true;
			else
				rangeChangeUp = false;
		}
	}

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

		// カメラのターゲット更新処理
	void UpdateCameraTarget(float elapsedTime);

	//-------------------------------------------------------------------//

		// カメラの状態更新処理
	void UpdateCameraState(float elapsedTime);

	// カメラの制限処理
	void CameraLimit();

	// デブッグカメラ
	bool UpdateDebugCamera(float elapsedTime);

	//---------------------------------------------------------------------------//

		// カメラのターゲットの変更更新処理
	bool UpdateTargetChange(float elapsedTime);

	// 単一軸のターゲット位置変更処理
	float UpdateTargetAxis(float target, float speed, bool targetChangeUp, float toTargetChangeChange, float elapsedTime);

	//---------------------------------------------------------------------------//

		// カメラの角度の変更更新処理
	bool UpdateAngleChange(float elapsedTime);

	// 単一軸の回転変更処理
	float UpdateAngleAxis(float angle, float speed, bool angleChangeUp, float toAngleChange, float elapsedTime);

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
	DirectorManager directorManager;

	float rollSpeed = DirectX::XMConvertToRadians(90);
	float minAngleX = DirectX::XMConvertToRadians(-90);
	float maxAngleX = DirectX::XMConvertToRadians(90);

	float height = 9.0f; // カメラの高さ
	float dist = 0.0f; // プレイヤーとターゲットの距離

	bool  tracking = false; // カメラをプレイヤー中心でターゲットに注目させるか

	/**********************************************************************************************/
		/*! ターゲット */

public:
	static DirectX::XMFLOAT3 target;

	static bool              targetChange;      // ターゲットの位置を変えるか
	static DirectX::XMFLOAT3 toTargetChange;    // ここまでターゲットの位置を変える
	static DirectX::XMFLOAT3 targetChangeSpeed; // ターゲットの位置を変える速さ

	struct TargetChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};
	static TargetChangeUp targetChangeUp; // ターゲットの位置を増やすか

	/**********************************************************************************************/
		/*! 角度 */

public:
	static DirectX::XMFLOAT3 angle;

	static bool              angleChange;      // 角度を変えるか
	static DirectX::XMFLOAT3 toAngleChange;    // ここまで角度を変える
	static DirectX::XMFLOAT3 angleChangeSpeed; // 角度を変える速さ

	struct AngleChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};
	static AngleChangeUp angleChangeUp; // 角度を増やすか

	/**********************************************************************************************/
		/*! カメラの範囲 */

private:
	float minRange = 2.0f;   // カメラ範囲の最小
	float maxRange = 999.0f; // カメラ範囲の最大

public:
	static float range; // カメラ範囲

	static bool  rangeChange;      // カメラの範囲を変えるか
	static float toRangeChange;    // ここまでカメラの範囲を変える
	static float rangeChangeSpeed; // カメラの範囲を変える速さ
	static bool  rangeChangeUp;    // カメラの範囲を増やすか

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

	//-----------------------------------------------------------------//

	bool cameraTargetPlayer_3D = false; // カメラのターゲットをプレイヤーにするか

	bool  cameraMovie = false; // カメラのムービー中か
	float movieTime = 0.0f;  // ムービーの時間

	//-----------------------------------------------------------------//

	std::unique_ptr<Fade> fade;
	bool bossFinishSettings = false; // 設定したか

	bool bossFinish = false; // ボスフィニッシュするか
	float bossFinishTimer = 0.0f;  // ボスフィニッシュ時間
};