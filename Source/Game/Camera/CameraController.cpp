#include "Game/Camera/CameraController.h"
#include "Game/Camera/Camera.h"
#include "Game/Character/Player.h"
#include "Input/Input.h"
#include <imgui.h>

// 更新処理

void CameraController::Update(float elapsedTime)
{
	/// X軸のカメラ回転を制限
	if (angle.x < minAngleX)
	{
		angle.x = minAngleX;
	}
	if (angle.x > maxAngleX)
	{
		angle.x = maxAngleX;
	}

	Player& player = Player::Instance();

	angle.y = player.GetAngle().y;

	// カメラの回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//{
	//	angle.x,-angle.y,
	//	angle.y,angle.x
	//};

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front; // カメラの向き
	DirectX::XMStoreFloat3(&front, Front);
	
	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	// カメラを揺らす更新処理
	UpdateCameraShake(elapsedTime);

	// カメラの視点と注視点を設定
	Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));

	//! カメラのムービー中の処理
	UpdateCameraMovieTimer(elapsedTime);
}

// カメラムービー時間処理
bool CameraController::UpdateCameraMovieTimer(float elapsedTime)
{
	// カメラがムービー中なら
	if (cameraMovie && movieTime > 0.0f)
	{
		movieTime -= elapsedTime;

		return true;
	}
	else
	{
		cameraMovie = false;
		movieTime = 0;
		return false;
	}
}

// カメラシェイク更新処理
bool CameraController::UpdateCameraShake(float elapsedTime)
{
	if (cameraShake && shakeTimer > 0.0f)
	{
		// 揺らす大きさが1より大きいなら処理を行う
		if (shakeFlag.x)
			target.x += (rand() % shakePower.x * 2 + 1) * elapsedTime;
		if (shakeFlag.y)
			target.y += (rand() % shakePower.y * 2 + 1) * elapsedTime;
		if (shakeFlag.z)
			target.z += (rand() % shakePower.z * 2 + 1) * elapsedTime;

		shakeTimer -= elapsedTime;

		return true;
	}
	else
		cameraShake = false;

	return false;
}


void CameraController::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::TreeNode("CameraController"))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::DragFloat3("Angle", &a.x);
			// ターゲット
			ImGui::InputFloat3("Target", &target.x);
			// RollSpeed
			ImGui::InputFloat("RollSpeed", &rollSpeed);
			// Range
			ImGui::InputFloat("Range", &range);

			ImGui::InputFloat("MinAngleX", &minAngleX);
			ImGui::InputFloat("MaxAngleX", &maxAngleX);

			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
		}
		ImGui::TreePop();
	}
}
