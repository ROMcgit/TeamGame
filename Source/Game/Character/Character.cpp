#include "Game/Character/Character.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Game/Stage/StageManager.h"
#include "Game/Camera/CameraController.h"
#include "Audio/SoundManager.h"
#include <imgui.h>
#include <random>

// 当たり判定の位置を設定
void Character::CollisionPosSettings()
{
	collisionPos = {
		position.x + collisionOffset.x,
		position.y + collisionOffset.y,
		position.z + collisionOffset.z
	};
}

// 行列更新処理
void Character::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// ３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

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
	hpDamageCount = 0;

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
}

// 死亡した時に呼ばれる
void Character::OnDead()
{
}

/*****************************************************************************************/

//! HP管理
void Character::HpControll(float elapsedTime)
{
	// HPが上限を超えないように
	if (hp > maxHp)       hp = maxHp;
	if (hpDamage > maxHp) hpDamage = maxHp;

	// ピンチなら色を変える(プレイヤー)
	if (hp < maxHp * 0.25f) hpSpriteColorP = { 1.0f, 0.4118f, 0.7059f };

	// ダメージ処理
	if (hpDamage > hp)
	{
		hpDamageCount += elapsedTime;
		if (hpDamageCount >= 0.8f)
		{
			hpDamageDirectorWaitCount -= elapsedTime;
			if (hpDamageDirectorWaitCount <= 0)
			{
				hpDamage--;
				hpDamageDirectorWaitCount = maxHpDamageDirectorWaitCount;
			}
		}
	}
	// 回復した時
	else if (hpDamage < hp) hpDamage = hp;

	// HPなどが0より下にならないようにする
	if (hp < 0) hp = 0;

	if (hpDamage == hp) hpDamageCount = 0;
}

//! HP演出
bool Character::HpDirector(int hpPlusNum, int maxDoNum)
{
	//! 演出をしないならHPの演出を完了する
	if (maxDoNum < 1)
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
			sound.PlaySound("HP演出", 1.0f);

		// 演出が終わったかまたはHPが最大なら
		if (doHpDirectorCount >= maxDoNum || (hp == maxHp))
		{
			sound.StopSound("HP演出");
			hpDirectorFinished = true;
		}

		hp += hpPlusNum;     // HPを増やす
		doHpDirectorCount++; // カウントを増やす
	}

	return true;
}

//! HPピンチ更新処理(敵のみ)
bool Character::UpdateHpPinchEnemy()
{
	//! HPが35%より大きいなら、処理を止める
	if (hp > maxHp * 0.35f)
		return false;

	//! HPが25%より大きいなら
	if (hp > maxHp * 0.25f)
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.1f, 0.3f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.3f);
	}
	//! HPが18%より大きいなら
	else if (hp > maxHp * 0.18f)
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.3f, 0.5f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.5f);
	}
	//! HPが8%より大きいなら
	else if (hp > maxHp * 0.08f)
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.5f, 0.7f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.7f);
	}
	else
	{
		//! エミッシブの強さが0なら
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.8f, 1.2f);
		else if (!emissiveStrengthChange)
			// エミッシブの強さを変更
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 1.2f);
	}

	return true;
}

//! HPシェイク
bool Character::UpdateHpShake(float elapsedTime)
{
	// カメラシェイクタイマーが0.1f以下なら
	if (hpShakeTimer <= 0.15f && hpShake)
	{
		hpSpritePos.y = hpSpriteShakePosY - 4.5f; // HPゲージのシェイク位置
		hpSpriteColor = { 1, 0.6f, 1 };           // HPゲージの色
		hpImagePos.y = hpImageShakePosY - 4.5f;  // HP画像のシェイク位置
		hpImageColor = { 1, 0, 0 };              // HP画像の色

		hpSpriteColorP = { 1, 0, 0 }; // HPゲージを赤色にする(プレイヤー)

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
bool Character::UpdateHpPhaseChange(float elapsedTime)
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
void Character::UpdateHpColorChange(float& hpImageColor, float& hpImageColorChangeSpeed, bool& hpImageColorUp, float elapsedTime)
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
void Character::UpdateHpSpriteChange(float& hpSpriteColor, float& hpSpriteColorChangeSpeed, bool& hpSpriteColorDown, float elapsedTime)
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

/***********************************************************************************************/

//! 加速度更新処理
void Character::UpdateVelocity(float elapsedTime)
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
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	// TODO:重力処理
	velocity.y += -gravity * elapsedFrame;
}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
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
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 法線ベクトル取得
			normal = hit.normal;

			// 地面に接地している
			position = hit.position;

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
void Character::UpdateHorizontalVelocity(float elapsedFrame)
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
void Character::UpdateHorizontalMove(float elapsedTime)
{
	// 水平速力量計算
	float velocityLengthXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		// TODO:水平移動値
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

/*****************************************************************************************/

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
void Character::Move3D(float vx, float vz, float speed)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	// 最大速度設定
	maxMoveSpeed = speed;
}

// 移動処理(2D)
void Character::Move2D(float vx, float speed)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;

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
void Character::Turn3D(float elapsedTime, float vx, float vz, float speed)
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
void Character::Turn2D(float elapsedTime, float vx, float speed)
{
	speed *= elapsedTime;

	// 進行ベクトルがゼロベクトルの場合は処理する必要なし
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 進行ベクトルを単位ベクトル化
	float moveVecLength = sqrtf(vx * vx);
	if (moveVec.x == 0) return;

	vx /= moveVecLength;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (vx * frontX); // 内積
	float rot = 1.0 - dot; // 補正値

	// 内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する

	speed *= rot;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (vx * frontZ);

	// 2Dの外積値が正の場合か負の場合によって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
		angle.y -= speed;
	else
		angle.y += speed;
}

/*****************************************************************************************************************/
/*! セッター */

// カメラのターゲットの位置の設定
void Character::SetCameraTargetChange(DirectX::XMFLOAT3 toCameraTarget, DirectX::XMFLOAT3 cameraTargetChangeSpeed)
{
	if (!CameraController::targetChange)
	{
		CameraController::targetChange = true;                    // カメラのターゲットの位置を変える
		CameraController::toTargetChange = toCameraTarget;          // ここまでターゲットの位置を変える
		CameraController::targetChangeSpeed = cameraTargetChangeSpeed; // ターゲットの位置を変える速さ

		// ターゲットXが目指す位置より小さい時
		if (CameraController::target.x < toCameraTarget.x)
			CameraController::targetChangeUp.x = true;
		else
			CameraController::targetChangeUp.x = false;

		// ターゲットYが目指す位置より小さい時
		if (CameraController::target.y < toCameraTarget.y)
			CameraController::targetChangeUp.y = true;
		else
			CameraController::targetChangeUp.y = false;

		// ターゲットZが目指す位置より小さい時
		if (CameraController::target.z < toCameraTarget.z)
			CameraController::targetChangeUp.z = true;
		else
			CameraController::targetChangeUp.z = false;
	}
}

// カメラの角度の設定
void Character::SetCameraAngleChange(DirectX::XMFLOAT3 toCameraAngle, DirectX::XMFLOAT3 cameraAngleChangeSpeed)
{
	if (!CameraController::angleChange)
	{
		CameraController::angleChange = true;                   // カメラの範囲を変える
		CameraController::toAngleChange = toCameraAngle;          // ここまで角度を変える
		CameraController::angleChangeSpeed = cameraAngleChangeSpeed; // 角度を変える速さ

		// 角度Xが目指す位置より小さい時
		if (CameraController::angle.x < toCameraAngle.x)
			CameraController::angleChangeUp.x = true;
		else
			CameraController::angleChangeUp.x = false;

		// 角度Yが目指す位置より小さい時
		if (CameraController::angle.y < toCameraAngle.y)
			CameraController::angleChangeUp.y = true;
		else
			CameraController::angleChangeUp.y = false;

		// 角度Zが目指す位置より小さい時
		if (CameraController::angle.z < toCameraAngle.z)
			CameraController::angleChangeUp.z = true;
		else
			CameraController::angleChangeUp.z = false;
	}
}

// カメラの範囲の変更の設定
void Character::SetCameraRangeChange(float toCameraRange, float cameraRangeChangeSpeed)
{
	if (!CameraController::rangeChange)
	{
		CameraController::rangeChange = true;                   // カメラの範囲を変える
		CameraController::toRangeChange = toCameraRange;          // ここまでカメラの範囲を変える
		CameraController::rangeChangeSpeed = cameraRangeChangeSpeed; // カメラの範囲を変える速さ

		// カメラの範囲が目指す大きさより小さい時
		if (CameraController::range < toCameraRange)
			CameraController::rangeChangeUp = true;
		else
			CameraController::rangeChangeUp = false;
	}
}

// カメラシェイク設定
void Character::SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower)
{
	if (!CameraController::cameraShake)
	{
		CameraController::cameraShake = true;             // カメラシェイクするか
		CameraController::cameraShakeTimer = cameraShakeTimer; // カメラシェイクを行う時間
		CameraController::cameraShakePower = cameraShakePower; // カメラシェイクの強さ
	}
}

/*****************************************************************************************************************/

//! ポストエフェクトのステータスを設定
void Character::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3 ColorFilter, float chromaticAberration)
{
	Camera::postEffect.contrast = contrast;            // コンストラクト
	Camera::postEffect.saturation = saturation;		  // サチュレーション
	Camera::postEffect.colorFilter = ColorFilter;		  // カラーフィルター
	Camera::postEffect.chromaticAberration = chromaticAberration; // クロマティックアベレーション
}

//! ポストエフェクトの変更の設定
void Character::SetPostEffectStatusChange(
	float toContrastChange,
	float contrastChangeSpeed,
	float toSaturationChange,
	float saturationChangeSpeed,
	const DirectX::XMFLOAT3 toColorFilterChange,
	const DirectX::XMFLOAT3 colorFilterChangeSpeed,
	float toChromaticAberrationChange,
	float chromaticAberrationChangeSpeed)
{
	Camera::postEffectChange = true; // ポストエフェクトのステータスを変える

	// コンストラクトの値を変えるように代入する
	Camera::contrastChangeSpeed = contrastChangeSpeed;
	Camera::toContrastChange = toContrastChange;
	// コンストラクトの値が目指す値より小さいなら
	if (Camera::postEffect.contrast < Camera::toContrastChange)
		Camera::contrastUp = true;
	else
		Camera::contrastUp = false;

	// サチュレーションの値を変えるように代入する
	Camera::saturationChangeSpeed = saturationChangeSpeed;
	Camera::toSaturationChange = toSaturationChange;
	// サチュレーションの値が目指す値より小さいなら
	if (Camera::postEffect.saturation < Camera::toSaturationChange)
		Camera::saturationUp = true;
	else
		Camera::saturationUp = false;

	// カラーフィルターの値を変えるように代入する
	Camera::colorFilterChangeSpeed.x = colorFilterChangeSpeed.x;
	Camera::colorFilterChangeSpeed.y = colorFilterChangeSpeed.y;
	Camera::colorFilterChangeSpeed.z = colorFilterChangeSpeed.z;
	Camera::toColorFilterChange.x = toColorFilterChange.x;
	Camera::toColorFilterChange.y = toColorFilterChange.y;
	Camera::toColorFilterChange.z = toColorFilterChange.z;
	// カラーフィルターの値を上昇させるか
	IncreaseColorFilterComponent(Camera::postEffect.colorFilter.x, Camera::toColorFilterChange.x, Camera::colorFilterUp.x);
	IncreaseColorFilterComponent(Camera::postEffect.colorFilter.y, Camera::toColorFilterChange.y, Camera::colorFilterUp.y);
	IncreaseColorFilterComponent(Camera::postEffect.colorFilter.z, Camera::toColorFilterChange.z, Camera::colorFilterUp.z);

	// クロマティックアベレーションの値を変えるように代入する
	Camera::chromaticAberrationChangeSpeed = chromaticAberrationChangeSpeed;
	Camera::toChromaticAberrationChange = toChromaticAberrationChange;
	// クロマティックアベレーションの値が目指す値より小さいなら
	if (Camera::postEffect.chromaticAberration < Camera::toChromaticAberrationChange)
		Camera::chromaticAberrationUp = true;
	else
		Camera::chromaticAberrationUp = false;
}

// カラーフィルターを上昇させるかの設定
void Character::IncreaseColorFilterComponent(float colorFilter, float toColorFilterChange, bool& colorFilterUp)
{
	// 目指す値より小さいなら
	if (colorFilter < toColorFilterChange)
		colorFilterUp = true;
	else
		colorFilterUp = false;
}

//! ジャンプ処理
void Character::Jump(float speed)
{
	// 上方向の力を設定
	velocity.y = speed;
}

/*****************************************************************************************************************/

//! 状態更新処理
void Character::UpdateCharacterState(float elapsedTime)
{
	// 行列更新処理
	UpdateTransform();

	// 当たり判定の位置を設定
	CollisionPosSettings();

	// HP管理
	HpControll(elapsedTime);

	// HPシェイク	更新処理
	UpdateHpShake(elapsedTime);

	// HPの形態変化演出
	UpdateHpPhaseChange(elapsedTime);

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

	// 位置変更更新処理
	UpdatePositionChange(elapsedTime);

	// 角度制限処理
	LimitAngle();

	// 角度変更更新処理
	UpdateAngleChange(elapsedTime);

	// 角度の回転の更新処理(特定の値は目指さず、指定された時間中に回転する)
	UpdateAngleRotation(elapsedTime);

	// スケール変更更新処理
	UpdateScaleChange(elapsedTime);

	// ムービー時間更新処理
	UpdateMovieTimer(elapsedTime);

	// ヒットストップ更新処理
	UpdateHitStop(elapsedTime);

	// 無敵時間更新処理
	UpdateInvincibleTimer(elapsedTime);

	// 防御力アップ時間更新
	UpdateDefensesUpTimer(elapsedTime);
}

//! マテリアルの色変更更新処理
bool Character::UpdateMaterialColorChange(float elapsedTime)
{
	//! マテリアルの色を変えないなら、処理を止める
	if (!materialColorChange)
		return false;


	//! マテリアルの色を変更
	materialColor.x = UpdateMaterialColorAxis(materialColor.x, materialColorChangeSpeed.x, materialColorChangeUp.x, toMaterialColorChange.x, elapsedTime);
	materialColor.y = UpdateMaterialColorAxis(materialColor.y, materialColorChangeSpeed.y, materialColorChangeUp.y, toMaterialColorChange.y, elapsedTime);
	materialColor.z = UpdateMaterialColorAxis(materialColor.z, materialColorChangeSpeed.z, materialColorChangeUp.z, toMaterialColorChange.z, elapsedTime);

	//! マテリアルの色変更が終わったら、処理を止める
	if (materialColor.x == toMaterialColorChange.x
		&& materialColor.y == toMaterialColorChange.y
		&& materialColor.z == toMaterialColorChange.z)
		materialColorChange = false;

	return true;
}

// 単一軸のマテリアルの色変更更新処理
float Character::UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime)
{
	// マテリアルの色変更
	materialColor += (materialColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((materialColorChangeUp && materialColor > toMaterialColorChange) || (!materialColorChangeUp && materialColor < toMaterialColorChange))
	{
		materialColor = toMaterialColorChange;
	}

	// マテリアルの色を返す
	return materialColor;
}

//! 不透明度変更更新処理
bool Character::UpdateOpacityChange(float elapsedTime)
{
	//! 不透明度を変更しないなら、処理を止める
	if (!opacityChange)
		return false;


	//! 不透明度を変更
	opacity += (opacityChangeUp ? 1 : -1) * opacityChangeSpeed * elapsedTime;

	//! 目指す値を超えた場合、目標値にする
	if ((opacityChangeUp && opacity > toOpacityChange) || (!opacityChangeUp && opacity < toOpacityChange))
		opacity = toOpacityChange;

	//! 不透明度の変更が終わったら
	if (opacity == toOpacityChange)
		opacityChange = false;

	return true;
}

//!エミッシブの色変更更新処理
bool Character::UpdateEmissiveColorChange(float elapsedTime)
{
	//!エミッシブの色を変えないなら、処理を止める
	if (!emissiveColorChange)
		return false;


	//!エミッシブの色を変更
	emissiveColor.x = UpdateEmissiveColorAxis(emissiveColor.x, emissiveColorChangeSpeed.x, emissiveColorChangeUp.x, toEmissiveColorChange.x, elapsedTime);
	emissiveColor.y = UpdateEmissiveColorAxis(emissiveColor.y, emissiveColorChangeSpeed.y, emissiveColorChangeUp.y, toEmissiveColorChange.y, elapsedTime);
	emissiveColor.z = UpdateEmissiveColorAxis(emissiveColor.z, emissiveColorChangeSpeed.z, emissiveColorChangeUp.z, toEmissiveColorChange.z, elapsedTime);

	//!エミッシブの色変更が終わったら、処理を止める
	if (emissiveColor.x == toEmissiveColorChange.x
		&& emissiveColor.y == toEmissiveColorChange.y
		&& emissiveColor.z == toEmissiveColorChange.z)
		emissiveColorChange = false;

	return true;
}

// 単一軸のエミシッブの色変更更新処理
float Character::UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime)
{
	//エミッシブの色変更
	emissiveColor += (emissiveColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((emissiveColorChangeUp && emissiveColor > toEmissiveColorChange) || (!emissiveColorChangeUp && emissiveColor < toEmissiveColorChange))
	{
		emissiveColor = toEmissiveColorChange;
	}

	//エミッシブの色を返す
	return emissiveColor;
}

//!エミッシブの強さ変更更新処理
bool Character::UpdateEmissiveStrengthChange(float elapsedTime)
{
	//!エミッシブの強さを変えないなら、処理を止める
	if (!emissiveStrengthChange)
		return false;

	//!エミッシブの強さを変更
	emissiveStrength += (emissiveStrengthChangeUp ? 1 : -1) * emissiveStrengthChangeSpeed * elapsedTime;

	//! 目指す値を超えた場合、目標値にする
	if ((emissiveStrengthChangeUp && emissiveStrength > toEmissiveStrengthChange) || (!emissiveStrengthChangeUp && emissiveStrength < toEmissiveStrengthChange))
		emissiveStrength = toEmissiveStrengthChange;

	//!エミッシブの強さの変更が終わったら、処理を止める
	if (emissiveStrength == toEmissiveStrengthChange)
		emissiveStrengthChange = false;

	return true;
}

//!エミッシブの形態変化演出更新処理
bool Character::UpdateEmissivePhaseChange(float elapsedTime)
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

//エミッシブの色変更更新処理
void Character::UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeSpeed, bool& emissiveColorDown, float elapsedTime)
{
	//! カラー
	emissiveColor += (emissiveColorDown ? -1 : 1) * emissiveColorChangeSpeed * elapsedTime;

	//! 色が最小値より小さい、または、最大値より大きいなら
	if (emissiveColor < hpImageColorMin || emissiveColor > hpImageColorMax)
	{
		// 数値を代入
		emissiveColor = std::clamp(emissiveColor,
			emissiveColorMin, emissiveColorMax);

		emissiveColorDown = !emissiveColorDown; // フラグを反転する
	}
}

//------------------------------------------------------------------------------------------//

//! 位置変更更新処理
bool Character::UpdatePositionChange(float elapsedTime)
{
	//! 位置を変えないなら、処理を止める
	if (!positionChange)
		return false;


	//! 位置変更(加速度が	ないなら)
	if (velocity.x == 0)
		position.x = UpdatePositionAxis(position.x, positionChangeSpeed.x, positionChangeSpeedIncrease.x, positionChangeUp.x, toPositionChange.x, elapsedTime);
	if (velocity.y == 0)
		position.y = UpdatePositionAxis(position.y, positionChangeSpeed.y, positionChangeSpeedIncrease.y, positionChangeUp.y, toPositionChange.y, elapsedTime);
	if (velocity.z == 0)
		position.z = UpdatePositionAxis(position.z, positionChangeSpeed.z, positionChangeSpeedIncrease.z, positionChangeUp.z, toPositionChange.z, elapsedTime);

	//! 目的の位置に来たら、または加速度が0じゃないなら
	if ((position.x == toPositionChange.x || velocity.x != 0)
		&& (position.y == toPositionChange.y || velocity.y != 0)
		&& (position.z == toPositionChange.z || velocity.z != 0))
		positionChange = false;

	return true;
}

// 単一軸の位置変更更新処理
float Character::UpdatePositionAxis(float position, float& speed, float positionChangeSpeedIncrease, bool positionChangeUp, float toPositionChange, float elapsedTime)
{
	// 位置変更
	position += (positionChangeUp ? 1 : -1) * (speed * elapsedTime);

	//! 増え続けるなら、速度を増やし続ける
	if (positionChangeSpeedContinuingToIncrease)
		speed += positionChangeSpeedIncrease * elapsedTime;

	// 目指す値を超えた場合、目標値にする
	if ((positionChangeUp && position > toPositionChange) || (!positionChangeUp && position < toPositionChange))
	{
		position = toPositionChange;
	}

	// 位置を返す
	return position;
}

//------------------------------------------------------------------------------------------//

//! 角度制限更新処理
void Character::LimitAngle()
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

//! 回転変更更新処理
bool Character::UpdateAngleChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange)
		return false;


	//! 角度変更
	angle.x = UpdateAngleAxis(angle.x, angleChangeSpeed.x, angleChangeUp.x, toAngleChange.x, elapsedTime);
	angle.y = UpdateAngleAxis(angle.y, angleChangeSpeed.y, angleChangeUp.y, toAngleChange.y, elapsedTime);
	angle.z = UpdateAngleAxis(angle.z, angleChangeSpeed.z, angleChangeUp.z, toAngleChange.z, elapsedTime);

	//! 角度変更が終わったら処理を止める
	if (angle.x == toAngleChange.x
		&& angle.y == toAngleChange.y
		&& angle.z == toAngleChange.z)
		angleChange = false;

	return true;
}

// 単一軸の角度変更更新処理
float Character::UpdateAngleAxis(float angle, float speed, bool angleChangeUp, float toAngleChange, float elapsedTime)
{
	// 角度変更
	angle += (angleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目指す値を超えた場合、目標値にする
	if ((angleChangeUp && angle > toAngleChange) || (!angleChangeUp && angle < toAngleChange))
		angle = toAngleChange;

	// 角度を返す
	return angle;
}

//! 角度の回転の更新処理(特定の値は目指さず、指定された時間中に回転する)
bool Character::UpdateAngleRotation(float elapsedTime)
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

//------------------------------------------------------------------------------------------//

//! スケール変更更新処理
bool Character::UpdateScaleChange(float elapsedTime)
{
	//! 大きさを変更しないなら、処理を止める
	if (!scaleChange)
		return false;


	//! 大きさ変更
	scale.x = UpdateScaleAxis(scale.x, scaleChangeSpeed.x, scaleChangeUp.x, toScaleChange.x, elapsedTime);
	scale.y = UpdateScaleAxis(scale.y, scaleChangeSpeed.y, scaleChangeUp.y, toScaleChange.y, elapsedTime);
	scale.z = UpdateScaleAxis(scale.z, scaleChangeSpeed.z, scaleChangeUp.z, toScaleChange.z, elapsedTime);


	// 大きさ変更が終わったら処理を止める
	if (scale.x == toScaleChange.x
		&& scale.y == toScaleChange.y
		&& scale.z == toScaleChange.z)
		scaleChange = false;

	return true;
}

// 単一軸のスケール変更更新処理
float Character::UpdateScaleAxis(float scale, float speed, bool scaleChangeUp, float toScaleChange, float elapsedTime)
{
	// 大きさ変更
	scale += (scaleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// 目標値を超えた場合、目標値にする
	if ((scaleChangeUp && scale > toScaleChange) || (!scaleChangeUp && scale < toScaleChange))
		scale = toScaleChange;

	// 大きさを返す
	return scale;
}

//------------------------------------------------------------------------------------------//

//! ムービーシーンの待ち時間処理
bool Character::UpdateMovieTimer(float elapsedTime)
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

//! ヒットストップ更新処理
bool Character::UpdateHitStop(float elapsedTime)
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

	//! カメラを少し揺らす
	SetCameraShake(0.05f, DirectX::XMINT3(5, 8, 5));

	return true;
}

//! 無敵時間更新
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
		invincibleTimer -= elapsedTime;
}

//! 防御力アップ時間更新
void Character::UpdateDefensesUpTimer(float elapsedTime)
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