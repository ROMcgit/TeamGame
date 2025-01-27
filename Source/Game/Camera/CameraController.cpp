#include "CameraController.h"
#include "Camera.h"
#include "Graphics/Fade.h"
#include "Game/Character/Director/DirectorManager.h"
#include "Game/Character/Director/CameraTarget.h"
#include "Input/Input.h"
#include "Audio/SoundManager.h"
#include "Other/Easing.h"
#include <imgui.h>

/*! どのイージングにするか */

//! ターゲット
CameraController::TargetChangeEasing CameraController::targetChangeEasing = CameraController::TargetChangeEasing::Linear;
//! 角度
CameraController::AngleChangeEasing CameraController::angleChangeEasing = CameraController::AngleChangeEasing::Linear;
//! 範囲
CameraController::RangeChangeEasing CameraController::rangeChangeEasing = CameraController::RangeChangeEasing::Linear;

//! ターゲット
DirectX::XMFLOAT3 CameraController::target = { 0, 0, 0 };

//! ターゲットの位置を変変えるか
bool CameraController::targetChange = false;
//! ターゲットの変更の開始の値
DirectX::XMFLOAT3 CameraController::startTargetChange = { 0, 0, 0 };
//! ここまでターゲットの位置を変える
DirectX::XMFLOAT3 CameraController::endTargetChange = { 0, 0, 0 };
//! ターゲットの位置を変える時間
float CameraController::targetChangeTime = 0.0f;
//! ターゲットの位置変更の経過時間
float CameraController::targetChangeCurrentTime = 0.0f;

/********************************************************************/

//! 角度
DirectX::XMFLOAT3 CameraController::angle = { DirectX::XMConvertToRadians(26),0,0 };

//! 角度を変えるか
bool CameraController::angleChange = false;
//! 角度の変更の開始の値
DirectX::XMFLOAT3 CameraController::startAngleChange = { 0, 0, 0 };
//! ここまで角度を変える
DirectX::XMFLOAT3 CameraController::endAngleChange = { 0, 0, 0 };
//! 角度を変える時間
float CameraController::angleChangeTime = 0.0f;
//! 角度変更の経過時間
float CameraController::angleChangeCurrentTime = 0.0f;

/********************************************************************/

//! カメラの範囲
float CameraController::range = 16.0f;

//! カメラの範囲を変えるか
bool CameraController::rangeChange = false;
//! カメラの範囲の変更の開始の値
float CameraController::startRangeChange = 0.0f;
//! ここまでカメラの範囲を変える
float CameraController::endRangeChange = 0.0f;
//! カメラの範囲を変える時間
float CameraController::rangeChangeTime = 0.0f;
//! カメラの範囲変更の経過時間
float CameraController::rangeChangeCurrentTime = 0.0f;

/********************************************************************/

//! カメラを揺らすか
bool CameraController::cameraShake = false;
//! 揺らす時間
float CameraController::cameraShakeTimer = 0.0f;
//! 揺らす大きさ
DirectX::XMINT3 CameraController::cameraShakePower = { 0, 0, 0 };

/********************************************************************/

//! デバッグカメラ
bool CameraController::debugCamera = false;
//! デバッグカメラ中に移動しないようにするか
bool CameraController::noMoveDebugCamera = false;

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

	debugCamera = false; // デバッグカメラ
	noMoveDebugCamera = false; // デバッグカメラ中に移動しないようにするか


	// フェードを生成
	fade = std::make_unique<Fade>();

	SoundManager::Instance().LoadSound("ボス撃破", "Data/Audio/Sound/Enemy/Defeat.wav");

#ifndef _DEBUG
	//! カメラのターゲットを生成
	std::unique_ptr<CameraTarget> cameraTarget = std::make_unique<CameraTarget>();
	DirectorManager::Instance().Register(std::move(cameraTarget));
#endif
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

	// 演出マネージャーの更新処理
	DirectorManager::Instance().Update(elapsedTime);

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
	UpdateCameraTargetTracking(elapsedTime);
}

// 描画処理
void CameraController::RenderCameraTarget(ID3D11DeviceContext* dc, Shader* shader)
{
	//! デバッグカメラ中に表示
	DirectorManager::Instance().Render(dc, shader);
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
		// デバッグカメラか
		ImGui::Checkbox("DebugCamera", &debugCamera);
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

// カメラの注目更新処理
void CameraController::UpdateCameraTargetTracking(float elapsedTime)
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

// デバッグカメラ
bool CameraController::UpdateDebugCamera(float elapsedTime)
{
#ifndef _DEBUG

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

	//! ボタンでターゲットの移動
	{
		Camera& camera = Camera::Instance();

		// カメラの前方向、右方向、上方向を取得
		DirectX::XMFLOAT3 front = camera.GetFront();
		DirectX::XMFLOAT3 right = camera.GetRight();
		DirectX::XMFLOAT3 up = camera.GetUp();

		// 移動方向のベクトルを初期化
		DirectX::XMFLOAT3 moveDirection = { 0.0f, 0.0f, 0.0f };

		// キー入力で移動方向を決定
		GamePad& gamePad = Input::Instance().GetGamePad();

		//! Shiftキーを押しているなら
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			// 移動速度            // 上昇速度
			targetMoveSpeed = targetUpSpeed = 15.0f;
		}
		else
		{
			// 移動速度            // 上昇速度
			targetMoveSpeed = targetUpSpeed = 8.0f;
		}

		//! 上昇
		if (gamePad.GetButtonHeld() & GamePad::BTN_UP && GetAsyncKeyState(VK_CONTROL) & 0x8000 && !(gamePad.GetButtonHeld() & GamePad::BTN_DOWN))
		{
			// そのままY座標を移動させる
			target.y += targetUpSpeed * elapsedTime;
		}
		//! 前進
		else if (gamePad.GetButtonHeld() & GamePad::BTN_UP && !(gamePad.GetButtonHeld() & GamePad::BTN_DOWN))
		{
			moveDirection.x += front.x;
			moveDirection.y += front.y;
			moveDirection.z += front.z;
		}

		//! 下降
		if (gamePad.GetButtonHeld() & GamePad::BTN_DOWN && GetAsyncKeyState(VK_CONTROL) & 0x8000 && !(gamePad.GetButtonHeld() & GamePad::BTN_UP))
		{
			// そのままY座標を移動させる
			target.y -= targetUpSpeed * elapsedTime;
		}
		// 後退
		else if (gamePad.GetButtonHeld() & GamePad::BTN_DOWN && !(gamePad.GetButtonHeld() & GamePad::BTN_UP))
		{
			moveDirection.x -= front.x;
			moveDirection.z -= front.z;
		}

		// 左に移動
		if (gamePad.GetButtonHeld() & GamePad::BTN_LEFT && !(gamePad.GetButtonHeld() & GamePad::BTN_RIGHT))
		{
			moveDirection.x -= right.x;
			moveDirection.z -= right.z;
		}
		// 右に移動
		else if (gamePad.GetButtonHeld() & GamePad::BTN_RIGHT && !(gamePad.GetButtonHeld() & GamePad::BTN_LEFT))
		{
			moveDirection.x += right.x;
			moveDirection.z += right.z;
		}

		// 移動方向がゼロでない場合に処理(↑↓だけで、斜め移動になるのを防ぐため、UPは除外)
		//! 長さの二乗
		float lengthSquared = moveDirection.x * moveDirection.x +
			moveDirection.z * moveDirection.z;

		if (lengthSquared > 0.0f)
		{
			// 正規化して速度を適用
			float length = std::sqrt(lengthSquared);
			moveDirection.x /= length;
			moveDirection.z /= length;

			target.x += moveDirection.x * targetMoveSpeed * elapsedTime;
			target.z += moveDirection.z * targetMoveSpeed * elapsedTime;
		}
	}

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

	return true;

#else

	return false;

#endif // !_DEBUG
}

// カメラの位置の変更更新処理
bool CameraController::UpdateTargetChange(float elapsedTime)
{
	//! ターゲットの位置を変えないなら、処理を止める
	if (!targetChange)
		return false;

	//! 経過時間を計測
	targetChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = targetChangeCurrentTime / targetChangeTime;

	switch (targetChangeEasing)
	{
	case TargetChangeEasing::Linear:
	{
		//! ターゲット変更
		target.x = Easing::Linear(startTargetChange.x, endTargetChange.x, t);
		target.y = Easing::Linear(startTargetChange.y, endTargetChange.y, t);
		target.z = Easing::Linear(startTargetChange.z, endTargetChange.z, t);
	}
	break;
	case TargetChangeEasing::EaseIn:
	{
		//! ターゲット変更
		target.x = Easing::EaseIn(startTargetChange.x, endTargetChange.x, t);
		target.y = Easing::EaseIn(startTargetChange.y, endTargetChange.y, t);
		target.z = Easing::EaseIn(startTargetChange.z, endTargetChange.z, t);
	}
	break;
	case TargetChangeEasing::EaseOut:
	{
		//! ターゲット変更
		target.x = Easing::EaseOut(startTargetChange.x, endTargetChange.x, t);
		target.y = Easing::EaseOut(startTargetChange.y, endTargetChange.y, t);
		target.z = Easing::EaseOut(startTargetChange.z, endTargetChange.z, t);
	}
	break;
	default:
		break;
	}

	//! 処理を止める
	if (t >= 1.0f)
		targetChange = false;

	return true;
}

//------------------------------------------------------------------------------------------//

// カメラの角度の変更更新処理
bool CameraController::UpdateAngleChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange)
		return false;


	//! 経過時間を計測
	angleChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = angleChangeCurrentTime / angleChangeTime;

	switch (angleChangeEasing)
	{
	case AngleChangeEasing::Linear:
	{
		//! 角度変更
		angle.x = Easing::Linear(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::Linear(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::Linear(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::EaseIn:
	{
		//! 角度変更
		angle.x = Easing::EaseIn(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseIn(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseIn(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::EaseOut:
	{
		//! 角度変更
		angle.x = Easing::EaseOut(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseOut(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	default:
		break;
	}

	//! 処理を止める
	if (t >= 1.0f)
		angleChange = false;

	return true;
}

//------------------------------------------------------------------------------------------//

// カメラの範囲の変更更新処理
bool CameraController::UpdateRangeChange(float elapsedTime)
{
	//! カメラの範囲を変えないなら、処理を止める
	if (!rangeChange)
		return false;


	//! 経過時間を計測
	rangeChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = rangeChangeCurrentTime / rangeChangeTime;

	switch (rangeChangeEasing)
	{
	case RangeChangeEasing::Linear:
	{
		//! 範囲変更
		range = Easing::Linear(startRangeChange, endRangeChange, t);
	}
	break;
	case RangeChangeEasing::EaseIn:
	{
		//! 範囲変更
		range = Easing::EaseIn(startRangeChange, endRangeChange, t);
	}
	break;
	case RangeChangeEasing::EaseOut:
	{
		//! 範囲変更
		range = Easing::EaseOut(startRangeChange, endRangeChange, t);
	}
	break;
	default:
		break;
	}

	// 値が同じなら処理をとめる
	if (t >= 1.0f)
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
			//! フェードを設定
			fade->SetFade(
				DirectX::XMFLOAT3(1, 0, 0),
				0.9f, 0.0f,
				0.5f, 0.2f);

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