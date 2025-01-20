#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include "Audio/SoundManager.h"
#include "Graphics/Fade.h"
#include <imgui.h>

//! ターゲット
DirectX::XMFLOAT3 CameraController::target = { 0, 0, 0 };

//! ターゲットの位置を変変えるか
bool CameraController::targetChange = false;
//! ここまでターゲットの位置を変える
DirectX::XMFLOAT3 CameraController::toTargetChange = { 0, 0, 0 };
//! ターゲットの位置を変える速さ
DirectX::XMFLOAT3 CameraController::targetChangeSpeed = { 0, 0, 0 };
//! ターゲットの位置を増やすか
CameraController::TargetChangeUp CameraController::targetChangeUp = { false, false, false };

/********************************************************************/

//! 角度
DirectX::XMFLOAT3 CameraController::angle = { DirectX::XMConvertToRadians(26),0,0 };

//! 角度を変えるか
bool CameraController::angleChange = false;
//! ここまで角度を変える
DirectX::XMFLOAT3 CameraController::toAngleChange = { 0, 0, 0 };
//! 角度を変える速さ
DirectX::XMFLOAT3 CameraController::angleChangeSpeed = { 0, 0, 0 };
//! 角度を増やすか
CameraController::AngleChangeUp CameraController::angleChangeUp = { false, false, false };

/********************************************************************/

//! カメラの範囲
float CameraController::range = 16.0f;

//! カメラの範囲を変えるか
bool CameraController::rangeChange = false;
//! ここまでカメラの範囲を変える
float CameraController::toRangeChange = 0.0f;
//! カメラの範囲を変える速さ
float CameraController::rangeChangeSpeed = 0.0f;
//! カメラの範囲を増やすか
bool CameraController::rangeChangeUp = false;

/********************************************************************/

//! カメラを揺らすか
bool CameraController::cameraShake = false;
//! 揺らす時間
float CameraController::cameraShakeTimer = 0.0f;
//! 揺らす大きさ
DirectX::XMINT3 CameraController::cameraShakePower = { 0, 0, 0 };


// コンストラクタ
CameraController::CameraController()
{
	//! 変数を初期化
	targetChange = false; // ターゲットの位置を変変えるか
	angleChange = false; // 角度を変えるか
	rangeChange = false; // カメラの範囲を変えるか

	cameraShake = false;       // カメラを揺らすか
	cameraShakeTimer = 0.0f;        // カメラを揺らす時間
	cameraShakePower = { 0, 0, 0 }; // 揺らす大きさ

	// フェードを生成
	fade = std::make_unique<Fade>();

	//SoundManager::Instance().LoadSound("ボス撃破", "Data/Audio/Sound/Enemy/Defeat.wav");
}

// デストラクタ
CameraController::~CameraController()
{
}

// 更新処理
void CameraController::Update(float elapsedTime)
{
	// フェード更新処理
	fade->Update(elapsedTime);

#if 1
	// カメラの回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	// カメラの状態更新処理
	UpdateCameraState(elapsedTime);

	// カメラの視点と注視点を設定
	Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
#endif
	// カメラのターゲット更新処理
	UpdateCameraTarget(elapsedTime);
}

// フェードを描画
void CameraController::FadeRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	// フェードを描画
	fade->Render(dc, graphics);
}

// カメラのデバッグGUI
void CameraController::DrawDebugGUI()
{
	if (ImGui::TreeNode("CameraController"))
	{
		// デバッグカメラの動きを止めるか
		ImGui::Checkbox("NoMoveDebugCamera", &noMoveDebugCamera);
		// 注目するか
		ImGui::Checkbox("Tracking", &tracking);

		if (ImGui::CollapsingHeader("Transform"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			// ターゲット
			ImGui::DragFloat3("Target", &target.x, 0.1f);

			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::DragFloat3("Angle", &a.x, 0.1f);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);

			// カメラの範囲
			ImGui::DragFloat("Range", &range, 0.1f);
		}

		if (ImGui::CollapsingHeader("Status"))
		{
			// 高さ
			ImGui::DragFloat("height", &height, 0.1f);
			// 距離
			ImGui::InputFloat("Dist", &dist);
		}

		ImGui::TreePop();
	}
}

// カメラのターゲット更新処理
void CameraController::UpdateCameraTarget(float elapsedTime)
{
}

// カメラの状態更新処理
void CameraController::UpdateCameraState(float elapsedTime)
{
	// カメラの制限処理
	CameraLimit();

	// デバッグカメラ
	UpdateDebugCamera(elapsedTime);

	// カメラの位置の変更更新処理
	UpdateTargetChange(elapsedTime);

	// カメラの角度の変更更新処理
	UpdateAngleChange(elapsedTime);

	// カメラの範囲の変更更新処理
	UpdateRangeChange(elapsedTime);

	// カメラシェイク更新処理
	UpdateCameraShake(elapsedTime);

	// カメラムービー時間処理
	UpdateCameraMovieTimer(elapsedTime);

	// 他の更新処理を止める更新処理
	UpdateBossFinish(elapsedTime);
}

// カメラの制限処理
void CameraController::CameraLimit()
{
	if (range < minRange) range = minRange;      // カメラ範囲最小
	else if (range > maxRange) range = maxRange; // カメラ範囲最大

	/// X軸のカメラ回転を制限
	if (angle.x < minAngleX) angle.x = minAngleX;
	if (angle.x > maxAngleX) angle.x = maxAngleX;

	// Y軸の回転値を-3.14～3.14に収める
	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI)  angle.y -= DirectX::XM_2PI;
}

// カメラをデバッグ
bool CameraController::UpdateDebugCamera(float elapsedTime)
{
#ifdef _DEBUG

	Mouse& mouse = Input::Instance().GetMouse();
	//! ImGuiを操作中は処理をしない
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
	{
		debugCamera = false; // デバッグカメラを解除
		noMoveDebugCamera = false; // デバッグカメラの動きを止めるのを解除する

		return false;
	}
	//! ゲーム画面をクリックしたら、デバッグカメラに切り替える
	else
	{
		debugCamera = true;  // デバッグカメラにする

		if (tracking)
			tracking = false; // カメラの注目を解除
	}

	//! デバッグカメラの動きを止めるか
	if (mouse.GetButtonDown() & Mouse::BTN_MIDDLE)
		noMoveDebugCamera = !noMoveDebugCamera;

	//! デバッグカメラを動かさないなら、ここで処理を止める
	if (noMoveDebugCamera) return false;


	//! マウス左クリック長押し
#if 1
	if (mouse.GetButtonHeld() & Mouse::BTN_LEFT)
	{
		// マウスの移動量に合わせてカメラを回転
		float deltaX = static_cast<float>(mouse.GetPositionX() - mouse.GetOldPositionX());
		float deltaY = static_cast<float>(mouse.GetPositionY() - mouse.GetOldPositionY());

		Camera& camera = Camera::Instance();
		// カメラの前方向と右方向と注視点を取得
		DirectX::XMFLOAT3 up = camera.GetUp();
		DirectX::XMFLOAT3 right = camera.GetRight();
		DirectX::XMFLOAT3 focus = camera.GetFocus();

		float speed = (mouseSensitivity * (range * 0.7f)) * elapsedTime;
		float x = deltaX * speed;
		float y = deltaY * speed;

		// 右方向
		focus.x -= right.x * x;
		focus.y -= right.y * x;
		focus.z -= right.z * x;

		// 上方向
		focus.x += up.x * y;
		focus.y += up.y * y;
		focus.z += up.z * y;

		//! ターゲット位置を設定
		target.x = focus.x;
		target.y = focus.y;
		target.z = focus.z;
	}
#endif

	// IMGUIのマウス入力値を使ってカメラ操作する
	ImGuiIO io = ImGui::GetIO();
	//! マウスホイール長押し
	if (io.MouseWheel != 0)
	{
		range -= io.MouseWheel * range * mouseZoomSpeed;
	}

	//! マウス右クリック長押し
	if (mouse.GetButtonHeld() & Mouse::BTN_RIGHT)
	{
		// マウスの移動量に合わせてカメラを回転
		int deltaX = mouse.GetPositionX() - mouse.GetOldPositionX();
		int deltaY = mouse.GetPositionY() - mouse.GetOldPositionY();

		angle.y += deltaX * mouseSensitivity * elapsedTime;
		angle.x += deltaY * mouseSensitivity * elapsedTime;
	}

	//! マウスの更新処理
	mouse.Update();

#endif // !_DEBUG

	return true;
}

// カメラの位置の変更更新処理
bool CameraController::UpdateTargetChange(float elapsedTime)
{
	//! ターゲットの位置を変えないなら、処理を止める
	if (!targetChange)
		return false;

	//! ターゲットの位置を変える
	angle.x = UpdateAngleAxis(target.x, targetChangeSpeed.x, targetChangeUp.x, toTargetChange.x, elapsedTime);
	angle.y = UpdateAngleAxis(target.y, targetChangeSpeed.y, targetChangeUp.y, toTargetChange.y, elapsedTime);
	angle.z = UpdateAngleAxis(target.z, targetChangeSpeed.z, targetChangeUp.z, toTargetChange.z, elapsedTime);

	//! 値が同じなら処理を終わる
	if (target.x == toTargetChange.x && target.y == toTargetChange.y && target.z == toTargetChange.z)
		targetChange = false;

	return true;
}

// 単一軸のターゲット位置変更処理
float CameraController::UpdateTargetAxis(float target, float speed, bool targetChangeUp, float toTargetChangeChange, float elapsedTime)
{
	// ターゲット
	target += (targetChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((targetChangeUp && target > toTargetChangeChange) || (!targetChangeUp && target < toTargetChangeChange))
		target = toTargetChangeChange;

	// 角度を返す
	return target;
}

//------------------------------------------------------------------------------------------//

// カメラの角度の変更更新処理
bool CameraController::UpdateAngleChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange)
		return false;

	//! 角度を変える
	angle.x = UpdateAngleAxis(angle.x, angleChangeSpeed.x, angleChangeUp.x, toAngleChange.x, elapsedTime);
	angle.y = UpdateAngleAxis(angle.y, angleChangeSpeed.y, angleChangeUp.y, toAngleChange.y, elapsedTime);
	angle.z = UpdateAngleAxis(angle.z, angleChangeSpeed.z, angleChangeUp.z, toAngleChange.z, elapsedTime);

	//! 値が同じなら処理を終わる
	if (angle.x == toAngleChange.x && angle.y == toAngleChange.y && angle.z == toAngleChange.z)
		angleChange = false;

	return true;
}

// 単一軸の回転変更処理
float CameraController::UpdateAngleAxis(float angle, float speed, bool angleChangeUp, float toAngleChange, float elapsedTime)
{
	// 角度を変更
	angle += (angleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((angleChangeUp && angle > toAngleChange) || (!angleChangeUp && angle < toAngleChange))
		angle = toAngleChange;

	// 角度を返す
	return angle;
}

//------------------------------------------------------------------------------------------//

// カメラの範囲の変更更新処理
bool CameraController::UpdateRangeChange(float elapsedTime)
{
	//! カメラの範囲を変えないなら、処理を止める
	if (!rangeChange)
		return false;

	//! カメラの範囲を変える
	range += (rangeChangeUp ? 1 : -1) * rangeChangeSpeed * elapsedTime;

	// 目標値を超えた場合、目標値にする
	if ((rangeChangeUp && range > toRangeChange) || (!rangeChangeUp && range < toRangeChange))
		range = toRangeChange;

	// 値が同じなら処理をとめる
	if (range == toRangeChange)
		rangeChange = false;

	return true;
}

// カメラシェイク更新処理
bool CameraController::UpdateCameraShake(float elapsedTime)
{
	if (cameraShake && cameraShakeTimer > 0.0f)
	{
		// 揺らす大きさが1より大きいなら処理を行う
		if (cameraShakePower.x > 0)
			target.x += (rand() % cameraShakePower.x * 2 + 1) * elapsedTime;
		if (cameraShakePower.y > 0)
			target.y += (rand() % cameraShakePower.y * 2 + 1) * elapsedTime;
		if (cameraShakePower.z > 0)
			target.z += (rand() % cameraShakePower.z * 2 + 1) * elapsedTime;

		cameraShakeTimer -= elapsedTime;

		return true;
	}
	else
		cameraShake = false;

	return false;
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

// 他の更新処理を止める更新処理
bool CameraController::UpdateBossFinish(float elapsedTime)
{
	if (bossFinish && bossFinishTimer > 0.0f)
	{
		// タイマーを減らす
		bossFinishTimer -= elapsedTime;

		// 設定をする
		if (!bossFinishSettings)
		{
			fade->SetFade(
				DirectX::XMFLOAT3(1, 0, 0),
				0.9f, 0.0f,
				2.0f, 0.2f);

			// カメラシェイク
			SetCameraShake(0.8f, DirectX::XMINT3(100, 100, 0));

			// 効果音
			SoundManager::Instance().PlaySound("ボス撃破", 0.5f);

			bossFinishSettings = true;
		}

		return true;
	}
	else
	{
		// 変数を設定
		bossFinish = false;
		bossFinishTimer = 0.0f;
		return false;
	}
}