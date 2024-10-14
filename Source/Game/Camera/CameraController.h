#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
	CameraController() {}
	~CameraController() {}

	// 更新処理
	void Update(float elapsedTime);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// カメラ範囲の設定
	void SetRange(float range) { this->range = range; }

	// カメラムービー時間の設定
	void SetCameraMovieTime(float movieTime)
	{
		this->cameraMovie = true;
		this->movieTime = movieTime;
	}

	// カメラムービー時間の更新処理
	bool UpdateCameraMovieTimer(float elapsedTime);

	// カメラがムービー中かの取得
	bool GetCameraMovie() { return cameraMovie; }

	// デバッグGUI描画
	void DrawDebugGUI();

private:
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	float             rollSpeed = DirectX::XMConvertToRadians(120);
	float             range = 10.0f;
	float             minAngleX = DirectX::XMConvertToRadians(-45);
	float             maxAngleX = DirectX::XMConvertToRadians(45);
	bool  cameraMovie = false; // カメラのムービー中か
	float movieTime = 0.0f;  // ムービーの時間
};