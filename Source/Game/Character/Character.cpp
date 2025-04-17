#include "Character.h"
#include "Game/Stage/StageManager.h"

#include "Game/Camera/Camera.h"
#include "Game/Camera/CameraController.h"

#include "Enemy/EnemyManager.h"

#include "Input/Input.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Other/Easing.h"
#include <imgui.h>
#include <random>

// ダメージを与える
bool Character::ApplyDamage(int damage, float invincibleTime, float defensesUpTime)
{
	// ダメージが0の場合は健康状態を変更する必要がない
	if (damage <= 0) return false;

	// 死亡している場合は健康状態を変更しない
	if (hp <= 0) return false;

	// 無敵時間ならダメージを与えない
	if (invincibleTimer > 0.0f) return false;

	// ダメージ演出までの時間
	hpDamageTimer = 0;

	// ダメージを計算
	this->damage = (damage + additionalDamage) - defenses;

	// ガードしている時
	if (isGuard)
		(int)this->damage *= 0.3f;

	// ダメージが0より小さい場合
	if (this->damage < 0)
		this->damage = 0; // ダメージを0にする

	// ダメージ処理
	hp -= this->damage;

	if (defensesUpTimer <= 0)
		defenses = rand() % 2 + 3;

	// 防御力アップ時間
	defensesUpTimer = defensesUpTime;

	// 無敵時間を代入
	invincibleTimer = invincibleTime;

	// 死亡通知
	if (hp <= 0 && hpDirectorFinished)
	{
		OnDead();
	}
	// ダメージ通知
	else
	{
		OnDamaged();
	}
	// 健康状態が変更した場合はtrueを返す
	return true;
};

/*****************************************************************************************/

/*! 移動関連 */
#if 1

// スティック入力値から移動ベクトルを習得(3D)
DirectX::XMFLOAT3 Character::GetMoveVec() const
{
	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルになるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルになるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

	if (cameraFrontLength > 0.0f)
	{
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// スティックの水平入力値をカメラ右入力に反映し、
	// スティックの垂直入力値をカメラ前方向に反映し、
	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec = { 0, 0, 0 };
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

// スティック入力値から移動ベクトルを習得(2D)
DirectX::XMFLOAT3 Character::GetMoveVec_2D() const
{
	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	// float ay = gamePad.GetAxisLY(); // Y軸の入力は無視する

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// 進行ベクトルを計算する（左右方向のみ）
	DirectX::XMFLOAT3 vec = { 0, 0, 0 };
	vec.x = cameraRightX * ax;
	vec.z = cameraRightZ * ax;
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

// 移動処理(3D)
void Character::Move3D_Player(float vx, float vz, float speed, bool slide)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	//! ゲームパッドのインスタンス取得
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 左スティックの入力値取得
	float axisLX = gamePad.GetAxisLX();
	float axisLY = gamePad.GetAxisLY();

	// スティックの入力の強さ（0.0～1.0）を計算
	float stickMagnitude = std::hypot(axisLX, axisLY);

	//! スライドステートじゃないなら
	if (!slide)
		// 最大速度設定
		maxMoveSpeed = abs(speed * stickMagnitude);
	//! スライドステートなら
	else
		// 最大速度設定
		maxMoveSpeed = speed;
}

// 移動処理(3D)
void Character::Move3D(float vx, float vz, float speed)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	// 最大速度設定
	maxMoveSpeed = speed;
}

// 移動処理(2D)
void Character::Move2D_Player(float vx, float speed, bool slide)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;

	//! ゲームパッドのインスタンス取得
	GamePad& gamePad = Input::Instance().GetGamePad();


	//! スライドステートじゃないなら
	if (!slide)
		// 最大速度設定
		maxMoveSpeed = abs(speed * gamePad.GetAxisLX());
	//! スライドステートなら
	else
		// 最大速度設定
		maxMoveSpeed = speed;
}

// 旋回処理(ゼロベクトルでも処理を止めない)
void Character::Turn3DNotCancel(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// 進行ベクトルを単位ベクトル化
	float moveVecLength = sqrtf(vx * vx + vz * vz);

	vx /= moveVecLength;
	vz /= moveVecLength;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (vx * frontX) + (vz * frontZ); // 内積
	float rot = 1.0 - dot; // 補正値

	// 内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する

	speed *= rot;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (vz * frontX) - (vx * frontZ);

	// 2Dの外積値が正の場合か負の場合によって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
	{
		angle.y += speed;
	}
	else
	{
		angle.y -= speed;
	}
}

// 旋回処理
void Character::Turn3D_Player(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// 進行ベクトルがゼロベクトルの場合は処理する必要なし
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 進行ベクトルを単位ベクトル化
	float moveVecLength = sqrtf(vx * vx + vz * vz);
	if (moveVec.x == 0 && moveVec.z == 0) return;

	vx /= moveVecLength;
	vz /= moveVecLength;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (vx * frontX) + (vz * frontZ); // 内積
	float rot = 1.0 - dot; // 補正値

	// 内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する

	speed *= rot;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (vz * frontX) - (vx * frontZ);

	// 2Dの外積値が正の場合か負の場合によって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
	{
		angle.y += speed;
	}
	else
	{
		angle.y -= speed;
	}
}

// 旋回処理(2D)
void Character::Turn2D_Player()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//! 左入力したなら
	if ((gamePad.GetButtonHeld() & GamePad::BTN_LEFT) && !(gamePad.GetButtonDown() & GamePad::BTN_RIGHT))
	{
		//! 角度変更を解除
		SetAngleChangeUnlock();

		//! 角度変更する
		SetAngleYChange(DirectX::XMConvertToRadians(-90), 0.1f);
	}
	//! 右入力したなら
	else if ((gamePad.GetButtonHeld() & GamePad::BTN_RIGHT) && !(gamePad.GetButtonDown() & GamePad::BTN_LEFT))
	{
		//! 角度変更を解除
		SetAngleChangeUnlock();

		//! 角度を変更する
		SetAngleYChange(DirectX::XMConvertToRadians(90), 0.1f);
	}
}

//! ジャンプ処理
void Character::Jump(float speed)
{
	// 上方向の力を設定
	velocity.y = speed;
}

#endif

/*****************************************************************************************************************/

/*! 敵関連 */
#if 1

// ターゲットに到着したかを判定
bool Character::IsArrivalTarget()
{
	// 目標地点までXZ平面での距離判定
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;
	if (distSq < (radius * radius) * 1.5f) // バグ防止のため、少し大きくする
		return true;

	return false;
}

// ターゲットの方を向く
void Character::DirectionToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 向きの処理
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// ターゲットの方へ一直線に進む
void Character::MoveToDirection_3D(float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move3D(vx, vz, moveSpeed * speedRate);
}

// 敵の破棄処理
void Character::Destroy_Enemy()
{
	auto& manager = EnemyManager::Instance();
	for (auto& enemy : manager.enemies)
	{
		if (enemy.get() == this) // `this` ポインタに対応する `shared_ptr` を探す
		{
			manager.Remove(enemy.get());
			break;
		}
	}
}

#endif