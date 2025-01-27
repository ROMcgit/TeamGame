#include "Game/Character/Character.h"
#include "Input/Input.h"
#include "Game/Stage/StageManager.h"
#include "Game/Camera/Camera.h"
#include "Game/Camera/CameraController.h"
#include "Audio/SoundManager.h"
#include "Other/Easing.h"
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

	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

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
void Character::SetCameraTargetChange(DirectX::XMFLOAT3 endCameraTarget, float cameraTargetChangeTime, int targetChangeEasing)
{
	if (!CameraController::targetChange)
	{
		CameraController::targetChange = true;                                                                  // カメラのターゲットの位置を変える
		CameraController::startTargetChange = CameraController::target;											  // カメラのターゲットの変更の開始の値
		CameraController::endTargetChange = endCameraTarget;                                                       // ここまでターゲットの位置を変える
		CameraController::targetChangeTime = cameraTargetChangeTime;                                                // ターゲットの位置を変える時間
		CameraController::targetChangeEasing = static_cast<CameraController::TargetChangeEasing>(targetChangeEasing); // どのイージングにするか
		CameraController::targetChangeCurrentTime = 0.0f;                                                                  // 経過時間をリセット
	}
}

// カメラの角度変更の設定
void Character::SetCameraAngleChange(DirectX::XMFLOAT3 endCameraAngle, float cameraAngleChangeTime, int angleChangeEasing)
{
	if (!CameraController::angleChange)
	{
		CameraController::angleChange = true;                                                                // カメラの角度を変える
		CameraController::startAngleChange = CameraController::angle;											   // カメラの角度の変更の開始の値
		CameraController::endAngleChange = endCameraAngle;                                                      // ここまで角度を変える
		CameraController::angleChangeTime = cameraAngleChangeTime;                                               // 角度を変える時間
		CameraController::angleChangeEasing = static_cast<CameraController::AngleChangeEasing>(angleChangeEasing); // どのイージングにするか
		CameraController::angleChangeCurrentTime = 0.0f;                                                                // 経過時間をリセット
	}
}

// カメラの範囲の変更の設定
void Character::SetCameraRangeChange(float endCameraRange, float cameraRangeChangeTime, int rangeChangeEasing)
{
	if (!CameraController::rangeChange)
	{
		CameraController::rangeChange = true;                                                                // カメラの範囲を変える
		CameraController::startRangeChange = CameraController::range;											    // カメラの範囲の変更の開始の値
		CameraController::endRangeChange = endCameraRange;                                                      // ここまでカメラの範囲を変える
		CameraController::rangeChangeTime = cameraRangeChangeTime;                                               // カメラの範囲を変える時間
		CameraController::rangeChangeEasing = static_cast<CameraController::RangeChangeEasing>(rangeChangeEasing); // どのイージングにするか
		CameraController::rangeChangeCurrentTime = 0.0f;                                                                // 経過時間をリセット
	}
}

// カメラシェイク設定
void Character::SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower)
{
	CameraController::cameraShake = true;             // カメラシェイクするか
	CameraController::cameraShakeTimer = cameraShakeTimer; // カメラシェイクを行う時間
	CameraController::cameraShakePower = cameraShakePower; // カメラシェイクの強さ
}

/*****************************************************************************************************************/

//! ポストエフェクトのステータスを設定
void Character::SetPostEffectStatusOnce(float contrast, float saturation, const DirectX::XMFLOAT3 ColorFilter, float chromaticAberration)
{
	Camera::postEffect.contrast = contrast;            // コンストラクト
	Camera::postEffect.saturation = saturation;		  // サチュレーション
	Camera::postEffect.colorFilter = ColorFilter;		  // カラーフィルター
	Camera::postEffect.chromaticAberration = chromaticAberration; // クロマティックアベレーション
}

// ポストエフェクトのステータスを設定
void Character::SetPostEffectStatusOnceResetChange(float endContrastChange, float contrastChangeTime, float endSaturationChange, float saturationChangeTime, const DirectX::XMFLOAT3 endColorFilterChange, float colorFilterChangeTime, float endChromaticAberrationChange, float chromaticAberrationChangeTime)
{
	//! 変更処理を一旦止める
	Camera::contrastChange = false; // コントラスト
	Camera::saturationChange = false; // サチュレーション
	Camera::colorFilterChange = false; // カラーフィルター
	Camera::chromaticAberrationChange = false; // クロマティックアベレーション

	//! コントラスト
	Camera::contrastChange = true;                        // コントラストの値を変える
	Camera::startContrastChange = Camera::postEffect.contrast; // コントラストの変更の開始の値
	Camera::endContrastChange = endContrastChange;           // ここまでコントラストの値を変える
	Camera::contrastChangeTime = contrastChangeTime;          // コントラストの値を変える時間
	Camera::contrastChangeCurrentTime = 0.0f;                        // 経過時間をリセット

	//! サチュレーション
	Camera::saturationChange = true;                          // サチュレーションの値を変える
	Camera::startSaturationChange = Camera::postEffect.saturation; // サチュレーションの変更の開始の値
	Camera::endSaturationChange = endSaturationChange;           // ここまでサチュレーションの値を変える
	Camera::saturationChangeTime = saturationChangeTime;          // サチュレーションの値を変える時間
	Camera::saturationChangeCurrentTime = 0.0f;                          // 経過時間をリセット

	//! カラーフィルターの値を変更していないなら
	Camera::colorFilterChange = true;                           // カラーフィルターの値を変える
	Camera::startColorFilterChange = Camera::postEffect.colorFilter; // カラーフィルターの変更の開始の値
	Camera::endColorFilterChange = endColorFilterChange;           // ここまでカラーフィルターの値を変える
	Camera::colorFilterChangeTime = colorFilterChangeTime;          // カラーフィルターの値を変える時間
	Camera::colorFilterChangeCurrentTime = 0.0f;                           // 経過時間をリセット

	//! クロマティックアベレーション
	Camera::chromaticAberrationChange = true;                                   // クロマティックアベレーションの値を変える
	Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // クロマティックアベレーションの変更の開始の値
	Camera::endChromaticAberrationChange = endChromaticAberrationChange;           // ここまでクロマティックアベレーションの値を変える
	Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // クロマティックアベレーションの値を変える時間
	Camera::chromaticAberrationChangeCurrentTime = 0.0f;                                   // 経過時間をリセット
}

// コントラスト変更の設定
void Character::SetContrastChange(float endContrastChange, float contrastChangeTime)
{
	if (!Camera::contrastChange)
	{
		Camera::contrastChange = true;                        // コントラストの値を変える
		Camera::startContrastChange = Camera::postEffect.contrast; // コントラストの変更の開始の値
		Camera::endContrastChange = endContrastChange;           // ここまでコントラストの値を変える
		Camera::contrastChangeTime = contrastChangeTime;          // コントラストの値を変える時間
		Camera::contrastChangeCurrentTime = 0.0f;                        // 経過時間をリセット
	}
}

// サチュレーション(色の彩度)変更の設定
void Character::SetSaturationChange(float endSaturationChange, float saturationChangeTime)
{
	if (!Camera::saturationChange)
	{
		Camera::saturationChange = true;                          // サチュレーションの値を変える
		Camera::startSaturationChange = Camera::postEffect.saturation; // サチュレーションの変更の開始の値
		Camera::endSaturationChange = endSaturationChange;           // ここまでサチュレーションの値を変える
		Camera::saturationChangeTime = saturationChangeTime;          // サチュレーションの値を変える時間
		Camera::saturationChangeCurrentTime = 0.0f;                          // 経過時間をリセット
	}
}

// カラーフィルター変更の設定
void Character::SetColorFilterChange(DirectX::XMFLOAT3& endColorFilterChange, float colorFilterChangeTime)
{
	if (!Camera::colorFilterChange)
	{
		Camera::colorFilterChange = true;                           // カラーフィルターの値を変える
		Camera::startColorFilterChange = Camera::postEffect.colorFilter; // カラーフィルターの変更の開始の値
		Camera::endColorFilterChange = endColorFilterChange;           // ここまでカラーフィルターの値を変える
		Camera::colorFilterChangeTime = colorFilterChangeTime;          // カラーフィルターの値を変える時間
		Camera::colorFilterChangeCurrentTime = 0.0f;                           // 経過時間をリセット
	}
}

// クロマティックアベレーション(色収差(色ズレ))変更の設定
void Character::SetChromaticAberrationChange(float endChromaticAberrationChange, float chromaticAberrationChangeTime)
{
	if (!Camera::chromaticAberrationChange)
	{
		Camera::chromaticAberrationChange = true;                                   // クロマティックアベレーションの値を変える
		Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // クロマティックアベレーションの変更の開始の値
		Camera::endChromaticAberrationChange = endChromaticAberrationChange;           // ここまでクロマティックアベレーションの値を変える
		Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // クロマティックアベレーションの値を変える時間
		Camera::chromaticAberrationChangeCurrentTime = 0.0f;                                   // 経過時間をリセット
	}
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


	//! 経過時間を計測
	materialColorChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float tX = materialColorChangeCurrentTime / materialColorChangeTime.x;
	float tY = materialColorChangeCurrentTime / materialColorChangeTime.y;
	float tZ = materialColorChangeCurrentTime / materialColorChangeTime.z;

	//! マテリアルの色を変更
	materialColor.x = Easing::Linear(materialColor.x, endMaterialColorChange.x, tX);
	materialColor.y = Easing::Linear(materialColor.y, endMaterialColorChange.y, tY);
	materialColor.z = Easing::Linear(materialColor.z, endMaterialColorChange.z, tZ);

	//! マテリアルの色の範囲
	materialColor.x = std::clamp(materialColor.x, 0.0f, 1.0f);
	materialColor.y = std::clamp(materialColor.y, 0.0f, 1.0f);
	materialColor.z = std::clamp(materialColor.z, 0.0f, 1.0f);

	//! マテリアルの色変更が終わったら、処理を止める
	if (tX >= 1.0f && tY >= 1.0f && tZ >= 1.0f)
		materialColorChange = false;

	return true;
}

//! 不透明度変更更新処理
bool Character::UpdateOpacityChange(float elapsedTime)
{
	//! 不透明度を変更しないなら、処理を止める
	if (!opacityChange)
		return false;


	//! 経過時間を計測
	opacityChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = opacityChangeCurrentTime / opacityChangeTime;

	//! 不透明度を変更
	opacity = Easing::Linear(startOpacityChange, endOpacityChange, t);

	//! 不透明度の変異
	opacity = std::clamp(opacity, 0.0f, 1.0f);

	//! 不透明度の変更が終わったら
	if (t >= 1.0f)
		opacityChange = false;

	return true;
}

//! エミッシブの色変更更新処理
bool Character::UpdateEmissiveColorChange(float elapsedTime)
{
	//! エミッシブの色を変えないなら、処理を止める
	if (!emissiveColorChange)
		return false;


	//! 経過時間を計測
	emissiveColorChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float tX = emissiveColorChangeCurrentTime / emissiveColorChangeTime.x;
	float tY = emissiveColorChangeCurrentTime / emissiveColorChangeTime.y;
	float tZ = emissiveColorChangeCurrentTime / emissiveColorChangeTime.z;

	//!エミッシブの色を変更
	emissiveColor.x = Easing::Linear(startEmissiveColorChange.x, endEmissiveColorChange.x, tX);
	emissiveColor.y = Easing::Linear(startEmissiveColorChange.y, endEmissiveColorChange.y, tY);
	emissiveColor.z = Easing::Linear(startEmissiveColorChange.z, endEmissiveColorChange.z, tZ);

	//! エミッシブの色の範囲
	emissiveColor.x = std::clamp(emissiveColor.x, 0.0f, 1.0f);
	emissiveColor.y = std::clamp(emissiveColor.y, 0.0f, 1.0f);
	emissiveColor.z = std::clamp(emissiveColor.z, 0.0f, 1.0f);

	//! 処理を止める
	if (tX >= 1.0f && tY >= 1.0f && tZ >= 1.0f);
	emissiveColorChange = false;

	return true;
}

//! エミッシブの強さ変更更新処理
bool Character::UpdateEmissiveStrengthChange(float elapsedTime)
{
	//! エミッシブの強さを変えないなら、処理を止める
	if (!emissiveStrengthChange)
		return false;


	//! 経過時間を計測
	emissiveStrengthChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float t = emissiveStrengthChangeCurrentTime / emissiveStrengthChangeTime;

	//! エミッシブの強さを変更
	emissiveStrength = Easing::Linear(startEmissiveStrengthChange, endEmissiveStrengthChange, t);

	//! エミッシブの強さの範囲
	emissiveStrength = std::clamp(emissiveStrength, 0.0f, 1.0f);

	//! 処理を止める
	if (t >= 1.0f)
		emissiveStrengthChange = false;

	return true;
}

//! エミッシブの形態変化演出更新処理
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

// エミッシブの色変更更新処理
void Character::UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeTime, bool& emissiveColorDown, float elapsedTime)
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

//------------------------------------------------------------------------------------------//

//! 位置変更更新処理
bool Character::UpdatePositionChange(float elapsedTime)
{
	//! 位置を変えないなら、処理を止める
	if (!positionChange)
		return false;


	//! 経過時間を計測
	positionChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float tX = positionChangeCurrentTime / positionChangeTime.x;
	float tY = positionChangeCurrentTime / positionChangeTime.y;
	float tZ = positionChangeCurrentTime / positionChangeTime.z;

	switch (positionChangeEasing)
	{
	case PositionChangeEasing::Linear:
	{
		//! 位置変更(加速度が	ないなら)
		if (velocity.x == 0)
			position.x = Easing::Linear(startPositionChange.x, endPositionChange.x, tX);
		if (velocity.y == 0)
			position.y = Easing::Linear(startPositionChange.y, endPositionChange.y, tY);
		if (velocity.z == 0)
			position.z = Easing::Linear(startPositionChange.z, endPositionChange.z, tZ);
	}
	break;
	case PositionChangeEasing::EaseIn:
	{
		//! 位置変更(加速度が	ないなら)
		if (velocity.x == 0)
			position.x = Easing::EaseIn(startPositionChange.x, endPositionChange.x, tX);
		if (velocity.y == 0)
			position.y = Easing::EaseIn(startPositionChange.y, endPositionChange.y, tY);
		if (velocity.z == 0)
			position.z = Easing::EaseIn(startPositionChange.z, endPositionChange.z, tZ);
	}
	break;
	case PositionChangeEasing::EaseOut:
	{
		//! 位置変更(加速度が	ないなら)
		if (velocity.x == 0)
			position.x = Easing::EaseOut(startPositionChange.x, endPositionChange.x, tX);
		if (velocity.y == 0)
			position.y = Easing::EaseOut(startPositionChange.y, endPositionChange.y, tY);
		if (velocity.z == 0)
			position.z = Easing::EaseOut(startPositionChange.z, endPositionChange.z, tZ);
	}
	break;
	default:
		break;
	}

	//! 処理を止める
	if (tX >= 1.0f && tY >= 1.0f && tZ >= 1.0f)
		positionChange = false;

	return true;
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

//! 角度変更更新処理
bool Character::UpdateAngleChange(float elapsedTime)
{
	//! 角度を変えないなら、処理を止める
	if (!angleChange)
		return false;


	//! 経過時間を計測
	angleChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float tX = angleChangeCurrentTime / angleChangeTime.x;
	float tY = angleChangeCurrentTime / angleChangeTime.y;
	float tZ = angleChangeCurrentTime / angleChangeTime.z;

	switch (angleChangeEasing)
	{
	case AngleChangeEasing::Linear:
	{
		//! 角度変更
		angle.x = Easing::Linear(startAngleChange.x, endAngleChange.x, tX);
		angle.y = Easing::Linear(startAngleChange.y, endAngleChange.y, tY);
		angle.z = Easing::Linear(startAngleChange.z, endAngleChange.z, tZ);
	}
	break;
	case AngleChangeEasing::EaseIn:
	{
		//! 角度変更
		angle.x = Easing::EaseIn(startAngleChange.x, endAngleChange.x, tX);
		angle.y = Easing::EaseIn(startAngleChange.y, endAngleChange.y, tY);
		angle.z = Easing::EaseIn(startAngleChange.z, endAngleChange.z, tZ);
	}
	break;
	case AngleChangeEasing::EaseOut:
	{
		//! 角度変更
		angle.x = Easing::EaseOut(startAngleChange.x, endAngleChange.x, tX);
		angle.y = Easing::EaseOut(startAngleChange.y, endAngleChange.y, tY);
		angle.z = Easing::EaseOut(startAngleChange.z, endAngleChange.z, tZ);
	}
	break;
	default:
		break;
	}

	//! 処理を止める
	if (tX >= 1.0f && tY >= 1.0f && tZ >= 1.0f)
		angleChange = false;

	return true;
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


	//! 経過時間を計測
	scaleChangeCurrentTime += elapsedTime;

	//! イージングタイム
	float tX = scaleChangeCurrentTime / scaleChangeTime.x;
	float tY = scaleChangeCurrentTime / scaleChangeTime.y;
	float tZ = scaleChangeCurrentTime / scaleChangeTime.z;

	switch (scaleChangeEasing)
	{
	case ScaleChangeEasing::Linear:
	{
		//! 大きさ変更
		scale.x = Easing::Linear(startScaleChange.x, endScaleChange.x, tX);
		scale.y = Easing::Linear(startScaleChange.y, endScaleChange.y, tY);
		scale.z = Easing::Linear(startScaleChange.z, endScaleChange.z, tZ);
	}
	break;
	case ScaleChangeEasing::EaseIn:
	{
		//! 大きさ変更
		scale.x = Easing::EaseIn(startScaleChange.x, endScaleChange.x, tX);
		scale.y = Easing::EaseIn(startScaleChange.y, endScaleChange.y, tY);
		scale.z = Easing::EaseIn(startScaleChange.z, endScaleChange.z, tZ);
	}
	break;
	case ScaleChangeEasing::EaseOut:
	{
		//! 大きさ変更
		scale.x = Easing::EaseOut(startScaleChange.x, endScaleChange.x, tX);
		scale.y = Easing::EaseOut(startScaleChange.y, endScaleChange.y, tY);
		scale.z = Easing::EaseOut(startScaleChange.z, endScaleChange.z, tZ);
	}
	break;
	default:
		break;
	}

	//! 処理を止める
	if (tX >= 1.0f && tY >= 1.0f && tZ >= 1.0f)
		scaleChange = false;

	return true;
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