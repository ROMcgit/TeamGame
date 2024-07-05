#include "Character.h"
#include "Input/Input.h"
#include "Camera.h"
#include "StageManager.h"

// 行列更新処理
void Character::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	
	// 回転行列を作成
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
#if 1
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Z * X * Y;
#endif

	// ３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::Move(float vx, float vz, float speed)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	// 最大速度設定
	maxMoveSpeed = speed;
}

void Character::OnDead()
{
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed)
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

// ジャンプ処理
//void Character::Jump(float speed)
//{
//	// 上方向の力を設定
//	velocity.y = speed;
//}

// 加速度更新処理
void Character::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	// 水平速力更新処理
	UpdateHorizontalVelocity(elapsedFrame);

	// 水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
}

// ダメージを与える
bool Character::ApplyDamage(int damage, float invincibleTime)
{
	// ダメージが0の場合は健康状態を変更する必要がない
	if (damage == 0) return false;

	// 死亡している場合は健康状態を変更しない
	if (health <= 0) return false;

	// 無敵時間なら
	if (invincibleTimer > 0) return false;

	// ダメージ処理
	health -= damage;

	// 無敵時間を代入
	invincibleTimer = invincibleTime;

	// 死亡通知
	if (health == 0)
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
}

// 衝撃を与える
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	// 速力に力を加える //

	// impulseをXMVECTORに変換
	DirectX::XMVECTOR impulseVec = DirectX::XMLoadFloat3(&impulse);

	// 現在の速度をXMVECTORに変換
	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);

	// impulseを速度に加算
	velocityVec = DirectX::XMVectorAdd(velocityVec, impulseVec);

	// 加算した結果をXMFLOAT3に戻してvelocityに格納
	DirectX::XMStoreFloat3(&velocity, velocityVec);

}

// 無敵時間更新
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

//// 垂直速力更新処理
//void Character::UpdateVerticalVelocity(float elapsedFrame)
//{
//	// 重力処理
//	velocity.y += gravity * elapsedFrame;
//}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
#if 0
	// 移動処理
	position.y += velocity.y * elapsedTime;

	// 地面判定
	if (position.y < 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;

		// 着地した
		if (!isGround)
		{
			OnLanding();
		}
		isGround = true;
	}
	else
	{
		isGround = false;
	}
#endif

	// 垂直方向の移動量
	float my = velocity.y * elapsedTime;

	// キャラクターのY軸方向となる法線ベクトル
	DirectX::XMFLOAT3 normal = { 0,1,0 };

	// 落下中
	if (my < 0.0f)
	{
		// レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		// レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

		// レイキャストによる地面判定
		HitResult hit;
		//if (Stage::Instance().RayCast(start, end, hit))
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 法線ベクトル取得
			normal = hit.normal;

			// 地面に接地している
			//position.y = hit.position.y;
			position = hit.position;

			// 回転(現状だと、前を向いてしまう
			//angle = hit.rotation;

			// 着地した
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;

			// 傾斜率の計算
			float adjustment = 1.0f - (slopeRate / DirectX::XM_PI); // 傾斜角に基づいた速度の調整率
			adjustment = std::max<float>(adjustment, 0.85f); // 最小速度調整率を設定して、速度が遅くなりすぎないようにする
			velocity.x *= adjustment;
			velocity.z *= adjustment;
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

#if 0
	// 地面の向きに沿うようにXZ軸回転
	{
		// 法線ベクトルを使用して地面の傾きを計算し、キャラクターの回転角を調整する
		DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&normal);
		DirectX::XMVECTOR rightVec = DirectX::XMVector3Cross(DirectX::XMVectorSet(0, 1, 0, 0), normalVec);
		rightVec = DirectX::XMVector3Normalize(rightVec);
		DirectX::XMVECTOR forwardVec = DirectX::XMVector3Cross(normalVec, rightVec);
		forwardVec = DirectX::XMVector3Normalize(forwardVec);

		// 回転行列を作成
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixIdentity();
		rotationMatrix.r[0] = rightVec;
		rotationMatrix.r[1] = normalVec;
		rotationMatrix.r[2] = forwardVec;

		// オイラー角を抽出
		DirectX::XMFLOAT3 euler;
		DirectX::XMStoreFloat3(&euler, DirectX::XMQuaternionRotationMatrix(rotationMatrix));

		// X軸とZ軸の回転角度を設定
		angle.x = euler.x;
		angle.z = euler.z;
	}
#endif
}

// 水平速力更新処理
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	// XZ平面の速力を減速する
	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMFLOAT3 velocityXZ = { velocity.x, 0.0f, velocity.z };  // XZ平面の速力
	DirectX::XMVECTOR velocityXZVec = DirectX::XMLoadFloat3(&velocityXZ);

	// XZ平面の速力
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(velocityXZVec));

	// XZ平面の速力を加速する
	{
		// 移動ベクトルがゼロベクトルでないなら加速する
		//DirectX::XMFLOAT3 moveVec = GetMoveVec();
		float moveVecLength = sqrt(moveVecX * moveVecX + moveVecZ * moveVecZ);

		// 摩擦による横方向の減速処理
		if (moveVecLength > 0.0f)
		{
			// 加速力
			float acceleration = this->acceleration * elapsedFrame;

			// 空中にいるときは摩擦力と加速力を減らす
			if (!isGround)
			{
				airControl = 0.5f;
				acceleration *= airControl;
				moveVecLength *= airControl;
			}
			else
			{
				airControl = 0;
			}

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

			// 下り坂でガタガタしないようにする
			if (slopeRate > 0.0f)
			{
				// 下り坂では速度が急激に増えないようにする
				float slopeAdjustment = 1.0f - (slopeRate / DirectX::XM_PIDIV2); // 傾斜角に基づいた速度の調整率
				slopeAdjustment = std::max<float>(slopeAdjustment, 0.8f); // 下り坂での最小速度調整率
				velocityXZVec = DirectX::XMVectorScale(velocityXZVec, slopeAdjustment);
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
void Character::UpdateHorizontalMove(float elapsedTime)
{
	// 水平速力量計算
	float velocityLengthXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		// 水平移動値
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = position;
		DirectX::XMFLOAT3 end = {
			position.x + mx,
			position.y,
			position.z + mz
		};

		// レイキャストによる壁判定
		HitResult hit;
		//if (Stage::Instance().RayCast(start, end, hit))
		if (StageManager::Instance().RayCast(start, end, hit))
		{
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);
            DirectX::XMVECTOR Projection = DirectX::XMVectorMultiply(Normal, Dot);

            // 補正位置の計算
            DirectX::XMVECTOR Correction = DirectX::XMVectorSubtract(Vec, Projection);
            DirectX::XMVECTOR CorrectedPosition = DirectX::XMVectorAdd(Start, Correction);

			// 補正位置の反映
			DirectX::XMFLOAT3 correctedPos;
            DirectX::XMStoreFloat3(&correctedPos, CorrectedPosition);
		
			position.x = correctedPos.x;
			position.z = correctedPos.z;
		}
		else
		{
			// 移動
			position.x += mx;
			position.z += mz;
		}

	}
}

// HP管理
void Character::HpControll()
{
	// HPが上限を超えないように
	if (health > maxHealth) health = maxHealth;

	// ダメージ処理
	if (damageHelth > health)
	{

		damageCount++;
		if (damageCount >= 60)
		{
			waitCount++;
			if (waitCount >= maxWaitCount)
			{
				damageHelth--;
				waitCount = 0;
			}
		}
	}
	// 回復した時
	else if (damageHelth < health) damageHelth = health;

	if (damageHelth == health) damageCount = 0;
}

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
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}