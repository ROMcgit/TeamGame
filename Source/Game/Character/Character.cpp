#include "Game/Character/Character.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Game/Stage/StageManager.h"
#include "Game/Camera/CameraController.h"
#include "Audio/SoundManager.h"
#include <imgui.h>
#include <random>

// �����蔻��̈ʒu��ݒ�
void Character::CollisionPosSettings()
{
	collisionPos = {
		position.x + collisionOffset.x,
		position.y + collisionOffset.y,
		position.z + collisionOffset.z
	};
}

// �s��X�V����
void Character::UpdateTransform()
{
	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// �R�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	// �v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

// �_���[�W��^����
bool Character::ApplyDamage(int damage, float invincibleTime, float defensesUpTime)
{
	// �_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
	if (damage <= 0) return false;

	// ���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (hp <= 0) return false;

	// ���G���ԂȂ�_���[�W��^���Ȃ�
	if (invincibleTimer > 0.0f) return false;

	// �_���[�W���o�܂ł̎���
	hpDamageCount = 0;

	// �_���[�W���v�Z
	this->damage = (damage + additionalDamage) - defenses;

	// �K�[�h���Ă��鎞
	if (isGuard)
		(int)this->damage *= 0.3f;

	// �_���[�W��0��菬�����ꍇ
	if (this->damage < 0)
		this->damage = 0; // �_���[�W��0�ɂ���

	// �_���[�W����
	hp -= this->damage;

	if (defensesUpTimer <= 0)
		defenses = rand() % 2 + 3;

	// �h��̓A�b�v����
	defensesUpTimer = defensesUpTime;

	// ���G���Ԃ���
	invincibleTimer = invincibleTime;

	// ���S�ʒm
	if (hp <= 0 && hpDirectorFinished)
	{
		OnDead();
	}
	// �_���[�W�ʒm
	else
	{
		OnDamaged();
	}
	// ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
	return true;
}

// ���S�������ɌĂ΂��
void Character::OnDead()
{
}

/*****************************************************************************************/

//! HP�Ǘ�
void Character::HpControll(float elapsedTime)
{
	// HP������𒴂��Ȃ��悤��
	if (hp > maxHp)       hp = maxHp;
	if (hpDamage > maxHp) hpDamage = maxHp;

	// �s���`�Ȃ�F��ς���(�v���C���[)
	if (hp < maxHp * 0.25f) hpSpriteColorP = { 1.0f, 0.4118f, 0.7059f };

	// �_���[�W����
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
	// �񕜂�����
	else if (hpDamage < hp) hpDamage = hp;

	// HP�Ȃǂ�0��艺�ɂȂ�Ȃ��悤�ɂ���
	if (hp < 0) hp = 0;

	if (hpDamage == hp) hpDamageCount = 0;
}

//! HP���o
bool Character::HpDirector(int hpPlusNum, int maxDoNum)
{
	//! ���o�����Ȃ��Ȃ�HP�̉��o����������
	if (maxDoNum < 1)
		hpDirectorFinished = true;

	// HP���o���I����Ă���Ȃ珈�������Ȃ�
	if (hpDirectorFinished)
		return false;

	//! HP�摜�̐�������
	if (hpSpriteHeight < 40)
	{
		hpSpriteHeight += 1;
		if (nameSpriteOpacity < 1.0f)
			nameSpriteOpacity += 0.04f;
	}
	else
	{
		//! ���ʉ�
		SoundManager& sound = SoundManager::Instance();
		sound.LoadSound("HP���o", "Data/Audio/Sound/Enemy/HpDirector.wav");
		if (!sound.IsPlayingSound("HP���o"))
			sound.PlaySound("HP���o", 1.0f);

		// ���o���I��������܂���HP���ő�Ȃ�
		if (doHpDirectorCount >= maxDoNum || (hp == maxHp))
		{
			sound.StopSound("HP���o");
			hpDirectorFinished = true;
		}

		hp += hpPlusNum;     // HP�𑝂₷
		doHpDirectorCount++; // �J�E���g�𑝂₷
	}

	return true;
}

//! HP�s���`�X�V����(�G�̂�)
bool Character::UpdateHpPinchEnemy()
{
	//! HP��35%���傫���Ȃ�A�������~�߂�
	if (hp > maxHp * 0.35f)
		return false;

	//! HP��25%���傫���Ȃ�
	if (hp > maxHp * 0.25f)
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.1f, 0.3f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.3f);
	}
	//! HP��18%���傫���Ȃ�
	else if (hp > maxHp * 0.18f)
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.3f, 0.5f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.5f);
	}
	//! HP��8%���傫���Ȃ�
	else if (hp > maxHp * 0.08f)
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.5f, 0.7f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.7f);
	}
	else
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.8f, 1.2f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 1.2f);
	}

	return true;
}

//! HP�V�F�C�N
bool Character::UpdateHpShake(float elapsedTime)
{
	// �J�����V�F�C�N�^�C�}�[��0.1f�ȉ��Ȃ�
	if (hpShakeTimer <= 0.15f && hpShake)
	{
		hpSpritePos.y = hpSpriteShakePosY - 4.5f; // HP�Q�[�W�̃V�F�C�N�ʒu
		hpSpriteColor = { 1, 0.6f, 1 };           // HP�Q�[�W�̐F
		hpImagePos.y = hpImageShakePosY - 4.5f;  // HP�摜�̃V�F�C�N�ʒu
		hpImageColor = { 1, 0, 0 };              // HP�摜�̐F

		hpSpriteColorP = { 1, 0, 0 }; // HP�Q�[�W��ԐF�ɂ���(�v���C���[)

		hpShakeTimer += elapsedTime;

		return true;
	}
	// ���ɖ߂�
	else
	{
		hpShakeTimer = 0.0f;
		hpShake = false;

		hpSpritePos.y = hpSpriteShakePosY; // HP�Q�[�W�̃V�F�C�N�ʒu
		hpImagePos.y = hpImageShakePosY;   // HP�摜�̃V�F�C�N�ʒu

		//! HP�̌`�ԕω����o�����Ă��Ȃ��Ȃ�
		if (!hpPhaseChange)
		{
			hpSpriteColor = { 1, 0, 1 }; // HP�Q�[�W�̐F
			hpImageColor = { 1, 1, 1 }; // HP�摜�̐F
			hpSpriteColorP = { 0, 1, 0 }; // HP�Q�[�W��ΐF�ɖ߂�(�v���C���[)
		}

		return false;
	}
}

//! HP�̌`�ԕω����o
bool Character::UpdateHpPhaseChange(float elapsedTime)
{
	//! �`�ԕω������Ȃ��Ȃ珈�����~�߂�
	if (!hpPhaseChange)
	{
		//! HP�V�F�C�N�����Ă��Ȃ��Ȃ�
		if (!hpShake)
		{
			hpImageColor = { 1, 1, 1 }; //! �F��߂�
			hpSpriteColor = { 1, 0, 1 }; //! ���F�ɖ߂�
		}

		return false;
	}

	//! �^�C�}�[�����炷
	if (hpPhaseChangeTimer > 0.0f)
		hpPhaseChangeTimer -= elapsedTime;
	else
		hpPhaseChange = false;

	//! HP�摜�̐F��ς���
	UpdateHpColorChange(hpImageColor.x, hpImageColorChangeSpeed.x, hpImageColorUp.x, elapsedTime);
	UpdateHpColorChange(hpImageColor.y, hpImageColorChangeSpeed.y, hpImageColorUp.y, elapsedTime);
	UpdateHpColorChange(hpImageColor.z, hpImageColorChangeSpeed.z, hpImageColorUp.z, elapsedTime);

	//! HP�Q�[�W�̐F��ς���
	UpdateHpSpriteChange(hpSpriteColor.y, hpSpriteColorChangeSpeed, hpSpriteColorDown, elapsedTime);

	return true;
}

// HP�摜�̐F�ύX����
void Character::UpdateHpColorChange(float& hpImageColor, float& hpImageColorChangeSpeed, bool& hpImageColorUp, float elapsedTime)
{
	//! �J���[
	hpImageColor += (hpImageColorUp ? 1 : -1) * hpImageColorChangeSpeed * elapsedTime;

	//! �F���ŏ��l��菬�����A�܂��́A�ő�l���傫���Ȃ�
	if (hpImageColor < hpImageColorMin || hpImageColor > hpImageColorMax)
	{
		// ���l����
		hpImageColor = std::clamp(hpImageColor,
			hpImageColorMin, hpImageColorMax);

		hpImageColorUp = !hpImageColorUp; // �t���O�𔽓]����
	}
}

// HP�Q�[�W�̐F�ύX�X�V����
void Character::UpdateHpSpriteChange(float& hpSpriteColor, float& hpSpriteColorChangeSpeed, bool& hpSpriteColorDown, float elapsedTime)
{
	//! �J���[
	hpSpriteColor += (hpSpriteColorDown ? -1 : 1) * hpSpriteColorChangeSpeed * elapsedTime;

	//! �ő�Ȃ�
	if (hpSpriteColor > hpSpriteColorMax)
	{
		hpSpriteColor = hpSpriteColorMax;
		hpSpriteColorDown = true;
	}
	//! �ŏ��Ȃ�
	else if (hpSpriteColor < hpSpriteColorMin)
	{
		hpSpriteColor = hpSpriteColorMin;
		hpSpriteColorDown = false;
	}
}

/***********************************************************************************************/

//! �����x�X�V����
void Character::UpdateVelocity(float elapsedTime)
{
	// �o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	// �������͍X�V����
	UpdateVerticalVelocity(elapsedFrame);

	// �������͍X�V����
	UpdateHorizontalVelocity(elapsedFrame);

	// �����ړ��X�V����
	UpdateVerticalMove(elapsedTime);

	// �����ړ��X�V����
	UpdateHorizontalMove(elapsedTime);
}

// �������͍X�V����
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	// TODO:�d�͏���
	velocity.y += -gravity * elapsedFrame;
}

// �����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
	// ���������̈ړ���
	float my = velocity.y * elapsedTime;

	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	DirectX::XMFLOAT3 normal = { 0,1,0 };

	// ������
	if (my < 0.0f)
	{
		// ���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		// ���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

		// ���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �@���x�N�g���擾
			normal = hit.normal;

			// �n�ʂɐڒn���Ă���
			position = hit.position;

			// ���n����
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			// �󒆂ɕ����Ă���
			position.y += my;
			isGround = false;
		}
	}
	// �㏸��
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
}

// �������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	/// XZ���ʂ̑��͂���������
	DirectX::XMVECTOR velocityVec = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMFLOAT3 velocityXZ = { velocity.x, 0.0f, velocity.z };  /// XZ���ʂ̑���
	DirectX::XMVECTOR velocityXZVec = DirectX::XMLoadFloat3(&velocityXZ);

	/// XZ���ʂ̑���
	float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(velocityXZVec));

	/// XZ���ʂ̑��͂���������
	{
		// �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrt(moveVecX * moveVecX + moveVecZ * moveVecZ);

		// ���C�ɂ�鉡�����̌�������
		if (moveVecLength > 0.0f)
		{
			// TODO:�L�����N�^�[�̉�����
			float acceleration = this->acceleration * elapsedFrame;

			DirectX::XMFLOAT3 moveV = DirectX::XMFLOAT3(moveVecX, 0, moveVecZ);
			DirectX::XMVECTOR moveXZVec = DirectX::XMLoadFloat3(&moveV);

			// �ړ��x�N�g���ɂ���������
			DirectX::XMVECTOR moveVecNormalized = DirectX::XMVector3Normalize(moveXZVec);
			DirectX::XMVECTOR accelerationVec = DirectX::XMVectorScale(moveVecNormalized, acceleration);
			velocityXZVec = DirectX::XMVectorAdd(velocityXZVec, accelerationVec);

			// �ő呬�͐���
			float newLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(velocityXZVec));
			if (newLength > maxMoveSpeed)
			{
				velocityXZVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(velocityXZVec), maxMoveSpeed);
			}
		}

		else
		{
			// �ړ��x�N�g�����[���̏ꍇ�͖��C�ɂ�錸���̂ݍs��
			DirectX::XMVECTOR frictionVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(velocityXZVec), -friction * elapsedFrame);

			// ���C�ŋt�����ɑ��x�����Ȃ��悤�ɂ���
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

	// �X�V���ꂽXZ���x��ۑ�
	DirectX::XMStoreFloat3(&velocityXZ, velocityXZVec);
	velocity.x = velocityXZ.x;
	velocity.z = velocityXZ.z;

	// �ړ��x�N�g�������Z�b�g
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

// �����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
	// �������͗ʌv�Z
	float velocityLengthXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		// TODO:�����ړ��l
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = position;
		DirectX::XMFLOAT3 end = {
			position.x + mx,
			position.y,
			position.z + mz
		};

		// ���C�L���X�g�ɂ��ǔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// �ǂ̖@��
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			// ���˃x�N�g����@���Ɏˉe
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);
			DirectX::XMVECTOR Projection = DirectX::XMVectorMultiply(Normal, Dot);

			// �␳�ʒu�̌v�Z
			DirectX::XMVECTOR Correction = DirectX::XMVectorSubtract(Vec, Projection);
			DirectX::XMVECTOR CorrectedPosition = DirectX::XMVectorAdd(Start, Correction);

			// �␳�ʒu�̔��f
			DirectX::XMFLOAT3 correctedPos;
			DirectX::XMStoreFloat3(&correctedPos, CorrectedPosition);

			position.x = correctedPos.x;
			position.z = correctedPos.z;
		}
		else
		{
			// �ړ�
			position.x += mx;
			position.z += mz;
		}
	}
}

/*****************************************************************************************/

// �X�e�B�b�N���͒l����ړ��x�N�g�����K��(3D)
DirectX::XMFLOAT3 Character::GetMoveVec() const
{
	// ���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɂȂ�悤�ɂ���

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

	if (cameraFrontLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// �X�e�B�b�N�̐������͒l���J�����E���͂ɔ��f���A
	// �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	// �i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec = { 0, 0, 0 };
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

// �X�e�B�b�N���͒l����ړ��x�N�g�����K��(2D)
DirectX::XMFLOAT3 Character::GetMoveVec_2D() const
{
	// ���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	// float ay = gamePad.GetAxisLY(); // Y���̓��͖͂�������

	// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

	if (cameraRightLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// �i�s�x�N�g�����v�Z����i���E�����̂݁j
	DirectX::XMFLOAT3 vec = { 0, 0, 0 };
	vec.x = cameraRightX * ax;
	vec.z = cameraRightZ * ax;
	// Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

// �ړ�����(3D)
void Character::Move3D(float vx, float vz, float speed)
{
	// �ړ������x�N�g����ݒ�
	moveVecX = vx;
	moveVecZ = vz;

	// �ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

// �ړ�����(2D)
void Character::Move2D(float vx, float speed)
{
	// �ړ������x�N�g����ݒ�
	moveVecX = vx;

	// �ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

// ���񏈗�(�[���x�N�g���ł��������~�߂Ȃ�)
void Character::Turn3DNotCancel(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// �i�s�x�N�g����P�ʃx�N�g����
	float moveVecLength = sqrtf(vx * vx + vz * vz);

	vx /= moveVecLength;
	vz /= moveVecLength;

	// ���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (vx * frontX) + (vz * frontZ); // ����
	float rot = 1.0 - dot; // �␳�l

	// ���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����

	speed *= rot;

	// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (vz * frontX) - (vx * frontZ);

	// 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	// ���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f)
	{
		angle.y += speed;
	}
	else
	{
		angle.y -= speed;
	}
}

// ���񏈗�
void Character::Turn3D(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// �i�s�x�N�g����P�ʃx�N�g����
	float moveVecLength = sqrtf(vx * vx + vz * vz);
	if (moveVec.x == 0 && moveVec.z == 0) return;

	vx /= moveVecLength;
	vz /= moveVecLength;

	// ���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (vx * frontX) + (vz * frontZ); // ����
	float rot = 1.0 - dot; // �␳�l

	// ���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����

	speed *= rot;

	// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (vz * frontX) - (vx * frontZ);

	// 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	// ���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f)
	{
		angle.y += speed;
	}
	else
	{
		angle.y -= speed;
	}
}

// ���񏈗�(2D)
void Character::Turn2D(float elapsedTime, float vx, float speed)
{
	speed *= elapsedTime;

	// �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// �i�s�x�N�g����P�ʃx�N�g����
	float moveVecLength = sqrtf(vx * vx);
	if (moveVec.x == 0) return;

	vx /= moveVecLength;

	// ���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (vx * frontX); // ����
	float rot = 1.0 - dot; // �␳�l

	// ���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����

	speed *= rot;

	// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (vx * frontZ);

	// 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	// ���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f)
		angle.y -= speed;
	else
		angle.y += speed;
}

/*****************************************************************************************************************/
/*! �Z�b�^�[ */

// �J�����̃^�[�Q�b�g�̈ʒu�̐ݒ�
void Character::SetCameraTargetChange(DirectX::XMFLOAT3 toCameraTarget, DirectX::XMFLOAT3 cameraTargetChangeSpeed)
{
	if (!CameraController::targetChange)
	{
		CameraController::targetChange = true;                    // �J�����̃^�[�Q�b�g�̈ʒu��ς���
		CameraController::toTargetChange = toCameraTarget;          // �����܂Ń^�[�Q�b�g�̈ʒu��ς���
		CameraController::targetChangeSpeed = cameraTargetChangeSpeed; // �^�[�Q�b�g�̈ʒu��ς��鑬��

		// �^�[�Q�b�gX���ڎw���ʒu��菬������
		if (CameraController::target.x < toCameraTarget.x)
			CameraController::targetChangeUp.x = true;
		else
			CameraController::targetChangeUp.x = false;

		// �^�[�Q�b�gY���ڎw���ʒu��菬������
		if (CameraController::target.y < toCameraTarget.y)
			CameraController::targetChangeUp.y = true;
		else
			CameraController::targetChangeUp.y = false;

		// �^�[�Q�b�gZ���ڎw���ʒu��菬������
		if (CameraController::target.z < toCameraTarget.z)
			CameraController::targetChangeUp.z = true;
		else
			CameraController::targetChangeUp.z = false;
	}
}

// �J�����̊p�x�̐ݒ�
void Character::SetCameraAngleChange(DirectX::XMFLOAT3 toCameraAngle, DirectX::XMFLOAT3 cameraAngleChangeSpeed)
{
	if (!CameraController::angleChange)
	{
		CameraController::angleChange = true;                   // �J�����͈̔͂�ς���
		CameraController::toAngleChange = toCameraAngle;          // �����܂Ŋp�x��ς���
		CameraController::angleChangeSpeed = cameraAngleChangeSpeed; // �p�x��ς��鑬��

		// �p�xX���ڎw���ʒu��菬������
		if (CameraController::angle.x < toCameraAngle.x)
			CameraController::angleChangeUp.x = true;
		else
			CameraController::angleChangeUp.x = false;

		// �p�xY���ڎw���ʒu��菬������
		if (CameraController::angle.y < toCameraAngle.y)
			CameraController::angleChangeUp.y = true;
		else
			CameraController::angleChangeUp.y = false;

		// �p�xZ���ڎw���ʒu��菬������
		if (CameraController::angle.z < toCameraAngle.z)
			CameraController::angleChangeUp.z = true;
		else
			CameraController::angleChangeUp.z = false;
	}
}

// �J�����͈̔͂̕ύX�̐ݒ�
void Character::SetCameraRangeChange(float toCameraRange, float cameraRangeChangeSpeed)
{
	if (!CameraController::rangeChange)
	{
		CameraController::rangeChange = true;                   // �J�����͈̔͂�ς���
		CameraController::toRangeChange = toCameraRange;          // �����܂ŃJ�����͈̔͂�ς���
		CameraController::rangeChangeSpeed = cameraRangeChangeSpeed; // �J�����͈̔͂�ς��鑬��

		// �J�����͈̔͂��ڎw���傫����菬������
		if (CameraController::range < toCameraRange)
			CameraController::rangeChangeUp = true;
		else
			CameraController::rangeChangeUp = false;
	}
}

// �J�����V�F�C�N�ݒ�
void Character::SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower)
{
	if (!CameraController::cameraShake)
	{
		CameraController::cameraShake = true;             // �J�����V�F�C�N���邩
		CameraController::cameraShakeTimer = cameraShakeTimer; // �J�����V�F�C�N���s������
		CameraController::cameraShakePower = cameraShakePower; // �J�����V�F�C�N�̋���
	}
}

/*****************************************************************************************************************/

//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
void Character::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3 ColorFilter, float chromaticAberration)
{
	Camera::postEffect.contrast = contrast;            // �R���X�g���N�g
	Camera::postEffect.saturation = saturation;		  // �T�`�����[�V����
	Camera::postEffect.colorFilter = ColorFilter;		  // �J���[�t�B���^�[
	Camera::postEffect.chromaticAberration = chromaticAberration; // �N���}�e�B�b�N�A�x���[�V����
}

//! �|�X�g�G�t�F�N�g�̕ύX�̐ݒ�
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
	Camera::postEffectChange = true; // �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ς���

	// �R���X�g���N�g�̒l��ς���悤�ɑ������
	Camera::contrastChangeSpeed = contrastChangeSpeed;
	Camera::toContrastChange = toContrastChange;
	// �R���X�g���N�g�̒l���ڎw���l��菬�����Ȃ�
	if (Camera::postEffect.contrast < Camera::toContrastChange)
		Camera::contrastUp = true;
	else
		Camera::contrastUp = false;

	// �T�`�����[�V�����̒l��ς���悤�ɑ������
	Camera::saturationChangeSpeed = saturationChangeSpeed;
	Camera::toSaturationChange = toSaturationChange;
	// �T�`�����[�V�����̒l���ڎw���l��菬�����Ȃ�
	if (Camera::postEffect.saturation < Camera::toSaturationChange)
		Camera::saturationUp = true;
	else
		Camera::saturationUp = false;

	// �J���[�t�B���^�[�̒l��ς���悤�ɑ������
	Camera::colorFilterChangeSpeed.x = colorFilterChangeSpeed.x;
	Camera::colorFilterChangeSpeed.y = colorFilterChangeSpeed.y;
	Camera::colorFilterChangeSpeed.z = colorFilterChangeSpeed.z;
	Camera::toColorFilterChange.x = toColorFilterChange.x;
	Camera::toColorFilterChange.y = toColorFilterChange.y;
	Camera::toColorFilterChange.z = toColorFilterChange.z;
	// �J���[�t�B���^�[�̒l���㏸�����邩
	IncreaseColorFilterComponent(Camera::postEffect.colorFilter.x, Camera::toColorFilterChange.x, Camera::colorFilterUp.x);
	IncreaseColorFilterComponent(Camera::postEffect.colorFilter.y, Camera::toColorFilterChange.y, Camera::colorFilterUp.y);
	IncreaseColorFilterComponent(Camera::postEffect.colorFilter.z, Camera::toColorFilterChange.z, Camera::colorFilterUp.z);

	// �N���}�e�B�b�N�A�x���[�V�����̒l��ς���悤�ɑ������
	Camera::chromaticAberrationChangeSpeed = chromaticAberrationChangeSpeed;
	Camera::toChromaticAberrationChange = toChromaticAberrationChange;
	// �N���}�e�B�b�N�A�x���[�V�����̒l���ڎw���l��菬�����Ȃ�
	if (Camera::postEffect.chromaticAberration < Camera::toChromaticAberrationChange)
		Camera::chromaticAberrationUp = true;
	else
		Camera::chromaticAberrationUp = false;
}

// �J���[�t�B���^�[���㏸�����邩�̐ݒ�
void Character::IncreaseColorFilterComponent(float colorFilter, float toColorFilterChange, bool& colorFilterUp)
{
	// �ڎw���l��菬�����Ȃ�
	if (colorFilter < toColorFilterChange)
		colorFilterUp = true;
	else
		colorFilterUp = false;
}

//! �W�����v����
void Character::Jump(float speed)
{
	// ������̗͂�ݒ�
	velocity.y = speed;
}

/*****************************************************************************************************************/

//! ��ԍX�V����
void Character::UpdateCharacterState(float elapsedTime)
{
	// �s��X�V����
	UpdateTransform();

	// �����蔻��̈ʒu��ݒ�
	CollisionPosSettings();

	// HP�Ǘ�
	HpControll(elapsedTime);

	// HP�V�F�C�N	�X�V����
	UpdateHpShake(elapsedTime);

	// HP�̌`�ԕω����o
	UpdateHpPhaseChange(elapsedTime);

	// �}�e���A���̐F�ύX�X�V����
	UpdateMaterialColorChange(elapsedTime);

	// �s�����x�ύX����
	UpdateOpacityChange(elapsedTime);

	//�G�~�b�V�u�̐F�ύX�X�V����
	UpdateEmissiveColorChange(elapsedTime);

	//�G�~�b�V�u�̋����ύX�X�V����
	UpdateEmissiveStrengthChange(elapsedTime);

	//�G�~�b�V�u�̌`�ԕω����o�X�V����
	UpdateEmissivePhaseChange(elapsedTime);

	// �ʒu�ύX�X�V����
	UpdatePositionChange(elapsedTime);

	// �p�x��������
	LimitAngle();

	// �p�x�ύX�X�V����
	UpdateAngleChange(elapsedTime);

	// �p�x�̉�]�̍X�V����(����̒l�͖ڎw�����A�w�肳�ꂽ���Ԓ��ɉ�]����)
	UpdateAngleRotation(elapsedTime);

	// �X�P�[���ύX�X�V����
	UpdateScaleChange(elapsedTime);

	// ���[�r�[���ԍX�V����
	UpdateMovieTimer(elapsedTime);

	// �q�b�g�X�g�b�v�X�V����
	UpdateHitStop(elapsedTime);

	// ���G���ԍX�V����
	UpdateInvincibleTimer(elapsedTime);

	// �h��̓A�b�v���ԍX�V
	UpdateDefensesUpTimer(elapsedTime);
}

//! �}�e���A���̐F�ύX�X�V����
bool Character::UpdateMaterialColorChange(float elapsedTime)
{
	//! �}�e���A���̐F��ς��Ȃ��Ȃ�A�������~�߂�
	if (!materialColorChange)
		return false;


	//! �}�e���A���̐F��ύX
	materialColor.x = UpdateMaterialColorAxis(materialColor.x, materialColorChangeSpeed.x, materialColorChangeUp.x, toMaterialColorChange.x, elapsedTime);
	materialColor.y = UpdateMaterialColorAxis(materialColor.y, materialColorChangeSpeed.y, materialColorChangeUp.y, toMaterialColorChange.y, elapsedTime);
	materialColor.z = UpdateMaterialColorAxis(materialColor.z, materialColorChangeSpeed.z, materialColorChangeUp.z, toMaterialColorChange.z, elapsedTime);

	//! �}�e���A���̐F�ύX���I�������A�������~�߂�
	if (materialColor.x == toMaterialColorChange.x
		&& materialColor.y == toMaterialColorChange.y
		&& materialColor.z == toMaterialColorChange.z)
		materialColorChange = false;

	return true;
}

// �P�ꎲ�̃}�e���A���̐F�ύX�X�V����
float Character::UpdateMaterialColorAxis(float materialColor, float speed, bool materialColorChangeUp, float toMaterialColorChange, float elapsedTime)
{
	// �}�e���A���̐F�ύX
	materialColor += (materialColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((materialColorChangeUp && materialColor > toMaterialColorChange) || (!materialColorChangeUp && materialColor < toMaterialColorChange))
	{
		materialColor = toMaterialColorChange;
	}

	// �}�e���A���̐F��Ԃ�
	return materialColor;
}

//! �s�����x�ύX�X�V����
bool Character::UpdateOpacityChange(float elapsedTime)
{
	//! �s�����x��ύX���Ȃ��Ȃ�A�������~�߂�
	if (!opacityChange)
		return false;


	//! �s�����x��ύX
	opacity += (opacityChangeUp ? 1 : -1) * opacityChangeSpeed * elapsedTime;

	//! �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((opacityChangeUp && opacity > toOpacityChange) || (!opacityChangeUp && opacity < toOpacityChange))
		opacity = toOpacityChange;

	//! �s�����x�̕ύX���I�������
	if (opacity == toOpacityChange)
		opacityChange = false;

	return true;
}

//!�G�~�b�V�u�̐F�ύX�X�V����
bool Character::UpdateEmissiveColorChange(float elapsedTime)
{
	//!�G�~�b�V�u�̐F��ς��Ȃ��Ȃ�A�������~�߂�
	if (!emissiveColorChange)
		return false;


	//!�G�~�b�V�u�̐F��ύX
	emissiveColor.x = UpdateEmissiveColorAxis(emissiveColor.x, emissiveColorChangeSpeed.x, emissiveColorChangeUp.x, toEmissiveColorChange.x, elapsedTime);
	emissiveColor.y = UpdateEmissiveColorAxis(emissiveColor.y, emissiveColorChangeSpeed.y, emissiveColorChangeUp.y, toEmissiveColorChange.y, elapsedTime);
	emissiveColor.z = UpdateEmissiveColorAxis(emissiveColor.z, emissiveColorChangeSpeed.z, emissiveColorChangeUp.z, toEmissiveColorChange.z, elapsedTime);

	//!�G�~�b�V�u�̐F�ύX���I�������A�������~�߂�
	if (emissiveColor.x == toEmissiveColorChange.x
		&& emissiveColor.y == toEmissiveColorChange.y
		&& emissiveColor.z == toEmissiveColorChange.z)
		emissiveColorChange = false;

	return true;
}

// �P�ꎲ�̃G�~�V�b�u�̐F�ύX�X�V����
float Character::UpdateEmissiveColorAxis(float emissiveColor, float speed, bool emissiveColorChangeUp, float toEmissiveColorChange, float elapsedTime)
{
	//�G�~�b�V�u�̐F�ύX
	emissiveColor += (emissiveColorChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((emissiveColorChangeUp && emissiveColor > toEmissiveColorChange) || (!emissiveColorChangeUp && emissiveColor < toEmissiveColorChange))
	{
		emissiveColor = toEmissiveColorChange;
	}

	//�G�~�b�V�u�̐F��Ԃ�
	return emissiveColor;
}

//!�G�~�b�V�u�̋����ύX�X�V����
bool Character::UpdateEmissiveStrengthChange(float elapsedTime)
{
	//!�G�~�b�V�u�̋�����ς��Ȃ��Ȃ�A�������~�߂�
	if (!emissiveStrengthChange)
		return false;

	//!�G�~�b�V�u�̋�����ύX
	emissiveStrength += (emissiveStrengthChangeUp ? 1 : -1) * emissiveStrengthChangeSpeed * elapsedTime;

	//! �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((emissiveStrengthChangeUp && emissiveStrength > toEmissiveStrengthChange) || (!emissiveStrengthChangeUp && emissiveStrength < toEmissiveStrengthChange))
		emissiveStrength = toEmissiveStrengthChange;

	//!�G�~�b�V�u�̋����̕ύX���I�������A�������~�߂�
	if (emissiveStrength == toEmissiveStrengthChange)
		emissiveStrengthChange = false;

	return true;
}

//!�G�~�b�V�u�̌`�ԕω����o�X�V����
bool Character::UpdateEmissivePhaseChange(float elapsedTime)
{
	//!�G�~�b�V�u�̌`�ԕω����o�����Ȃ��Ȃ�A�������~�߂�
	if (!emissivePhaseChange)
		return false;


	//! �^�C�}�[�����炷
	if (emissivePhaseChangeTimer > 0.0f)
		emissivePhaseChangeTimer -= elapsedTime;
	else
	{
		emissiveColor = { 0, 0, 0 }; //�G�~�b�V�u�J���[
		emissiveStrength = 0.0f;        //�G�~�b�V�u�̋���

		emissivePhaseChange = false;
	}

	//!�G�~�b�V�u�̐F�ύX�X�V����
	UpdateEmissiveColorPhaseChange(emissiveColor.x, emissiveColorPhaseChangeSpeed.x, emissiveColorChangeDown.x, elapsedTime);
	UpdateEmissiveColorPhaseChange(emissiveColor.y, emissiveColorPhaseChangeSpeed.y, emissiveColorChangeDown.y, elapsedTime);
	UpdateEmissiveColorPhaseChange(emissiveColor.z, emissiveColorPhaseChangeSpeed.z, emissiveColorChangeDown.z, elapsedTime);

	return true;
}

//�G�~�b�V�u�̐F�ύX�X�V����
void Character::UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeSpeed, bool& emissiveColorDown, float elapsedTime)
{
	//! �J���[
	emissiveColor += (emissiveColorDown ? -1 : 1) * emissiveColorChangeSpeed * elapsedTime;

	//! �F���ŏ��l��菬�����A�܂��́A�ő�l���傫���Ȃ�
	if (emissiveColor < hpImageColorMin || emissiveColor > hpImageColorMax)
	{
		// ���l����
		emissiveColor = std::clamp(emissiveColor,
			emissiveColorMin, emissiveColorMax);

		emissiveColorDown = !emissiveColorDown; // �t���O�𔽓]����
	}
}

//------------------------------------------------------------------------------------------//

//! �ʒu�ύX�X�V����
bool Character::UpdatePositionChange(float elapsedTime)
{
	//! �ʒu��ς��Ȃ��Ȃ�A�������~�߂�
	if (!positionChange)
		return false;


	//! �ʒu�ύX(�����x��	�Ȃ��Ȃ�)
	if (velocity.x == 0)
		position.x = UpdatePositionAxis(position.x, positionChangeSpeed.x, positionChangeSpeedIncrease.x, positionChangeUp.x, toPositionChange.x, elapsedTime);
	if (velocity.y == 0)
		position.y = UpdatePositionAxis(position.y, positionChangeSpeed.y, positionChangeSpeedIncrease.y, positionChangeUp.y, toPositionChange.y, elapsedTime);
	if (velocity.z == 0)
		position.z = UpdatePositionAxis(position.z, positionChangeSpeed.z, positionChangeSpeedIncrease.z, positionChangeUp.z, toPositionChange.z, elapsedTime);

	//! �ړI�̈ʒu�ɗ�����A�܂��͉����x��0����Ȃ��Ȃ�
	if ((position.x == toPositionChange.x || velocity.x != 0)
		&& (position.y == toPositionChange.y || velocity.y != 0)
		&& (position.z == toPositionChange.z || velocity.z != 0))
		positionChange = false;

	return true;
}

// �P�ꎲ�̈ʒu�ύX�X�V����
float Character::UpdatePositionAxis(float position, float& speed, float positionChangeSpeedIncrease, bool positionChangeUp, float toPositionChange, float elapsedTime)
{
	// �ʒu�ύX
	position += (positionChangeUp ? 1 : -1) * (speed * elapsedTime);

	//! ����������Ȃ�A���x�𑝂₵������
	if (positionChangeSpeedContinuingToIncrease)
		speed += positionChangeSpeedIncrease * elapsedTime;

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((positionChangeUp && position > toPositionChange) || (!positionChangeUp && position < toPositionChange))
	{
		position = toPositionChange;
	}

	// �ʒu��Ԃ�
	return position;
}

//------------------------------------------------------------------------------------------//

//! �p�x�����X�V����
void Character::LimitAngle()
{
	/// X���̉�]�l��-3.14�`3.14�Ɏ��߂�
	if (angle.x < -DirectX::XM_PI) angle.x += DirectX::XM_2PI;
	if (angle.x > DirectX::XM_PI)  angle.x -= DirectX::XM_2PI;

	// Y���̉�]�l��-3.14�`3.14�Ɏ��߂�
	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI)  angle.y -= DirectX::XM_2PI;

	// Z���̉�]�l��-3.14�`3.14�Ɏ��߂�
	if (angle.z < -DirectX::XM_PI) angle.z += DirectX::XM_2PI;
	if (angle.z > DirectX::XM_PI)  angle.z -= DirectX::XM_2PI;
}

//------------------------------------------------------------------------------------------//

//! ��]�ύX�X�V����
bool Character::UpdateAngleChange(float elapsedTime)
{
	//! �p�x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!angleChange)
		return false;


	//! �p�x�ύX
	angle.x = UpdateAngleAxis(angle.x, angleChangeSpeed.x, angleChangeUp.x, toAngleChange.x, elapsedTime);
	angle.y = UpdateAngleAxis(angle.y, angleChangeSpeed.y, angleChangeUp.y, toAngleChange.y, elapsedTime);
	angle.z = UpdateAngleAxis(angle.z, angleChangeSpeed.z, angleChangeUp.z, toAngleChange.z, elapsedTime);

	//! �p�x�ύX���I������珈�����~�߂�
	if (angle.x == toAngleChange.x
		&& angle.y == toAngleChange.y
		&& angle.z == toAngleChange.z)
		angleChange = false;

	return true;
}

// �P�ꎲ�̊p�x�ύX�X�V����
float Character::UpdateAngleAxis(float angle, float speed, bool angleChangeUp, float toAngleChange, float elapsedTime)
{
	// �p�x�ύX
	angle += (angleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((angleChangeUp && angle > toAngleChange) || (!angleChangeUp && angle < toAngleChange))
		angle = toAngleChange;

	// �p�x��Ԃ�
	return angle;
}

//! �p�x�̉�]�̍X�V����(����̒l�͖ڎw�����A�w�肳�ꂽ���Ԓ��ɉ�]����)
bool Character::UpdateAngleRotation(float elapsedTime)
{
	//! �p�x����]���Ȃ��Ȃ�A�������~�߂�
	if (!angleRotation)
		return false;

	//! ��]���Ԃ�0���傫���Ȃ�
	if (rotationTime > 0.0f)
		// ��]���Ԃ����炷
		rotationTime -= elapsedTime;
	else
		angleRotation = false;

	//! ��]����
	angle.x += angleRotationSpeed.x * elapsedTime;
	angle.y += angleRotationSpeed.y * elapsedTime;
	angle.z += angleRotationSpeed.z * elapsedTime;

	return true;
}

//------------------------------------------------------------------------------------------//

//! �X�P�[���ύX�X�V����
bool Character::UpdateScaleChange(float elapsedTime)
{
	//! �傫����ύX���Ȃ��Ȃ�A�������~�߂�
	if (!scaleChange)
		return false;


	//! �傫���ύX
	scale.x = UpdateScaleAxis(scale.x, scaleChangeSpeed.x, scaleChangeUp.x, toScaleChange.x, elapsedTime);
	scale.y = UpdateScaleAxis(scale.y, scaleChangeSpeed.y, scaleChangeUp.y, toScaleChange.y, elapsedTime);
	scale.z = UpdateScaleAxis(scale.z, scaleChangeSpeed.z, scaleChangeUp.z, toScaleChange.z, elapsedTime);


	// �傫���ύX���I������珈�����~�߂�
	if (scale.x == toScaleChange.x
		&& scale.y == toScaleChange.y
		&& scale.z == toScaleChange.z)
		scaleChange = false;

	return true;
}

// �P�ꎲ�̃X�P�[���ύX�X�V����
float Character::UpdateScaleAxis(float scale, float speed, bool scaleChangeUp, float toScaleChange, float elapsedTime)
{
	// �傫���ύX
	scale += (scaleChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڕW�l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((scaleChangeUp && scale > toScaleChange) || (!scaleChangeUp && scale < toScaleChange))
		scale = toScaleChange;

	// �傫����Ԃ�
	return scale;
}

//------------------------------------------------------------------------------------------//

//! ���[�r�[�V�[���̑҂����ԏ���
bool Character::UpdateMovieTimer(float elapsedTime)
{
	// ���[�r�[�V�[���Ŗ����Ȃ�A�������~�߂�
	if (!movieScene)
		return false;

	// �҂����Ԃ��I�������
	if (movieTime <= 0.0f)
		movieScene = false;

	movieTime -= elapsedTime;

	return true;
}

//! �q�b�g�X�g�b�v�X�V����
bool Character::UpdateHitStop(float elapsedTime)
{
	//! �q�b�g�X�g�b�v����Ȃ��Ȃ�A�������~�߂�
	if (!hitStop)
	{
		//! �ʏ펞��elapsedTime�ɐݒ�
		hitStopElapsedTime = elapsedTime;

		//! �q�b�g�X�g�b�v�^�C����ݒ�
		hitStopSeconds = hitStopMaxSeconds;

		return false;
	}

	//! �q�b�g�X�g�b�v�̎��Ԃ�0���傫���Ȃ�A���Ԃ����炷
	if (hitStopSeconds > 0)
		hitStopSeconds -= elapsedTime;
	else
	{
		hitStopSeconds = 0;         // 0�ɂ���
		hitStop = false;     // �q�b�g�X�g�b�v������
	}

	//! �q�b�g�X�g�b�v���݂̎��Ԃ�ݒ�
	hitStopElapsedTime = (1.0f - (hitStopSeconds / hitStopMaxSeconds)) * elapsedTime;

	//! �J�����������h�炷
	SetCameraShake(0.05f, DirectX::XMINT3(5, 8, 5));

	return true;
}

//! ���G���ԍX�V
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
		invincibleTimer -= elapsedTime;
}

//! �h��̓A�b�v���ԍX�V
void Character::UpdateDefensesUpTimer(float elapsedTime)
{
	if (defensesUpTimer > 0.0f)
	{
		defensesUpTimer -= elapsedTime;
	}
	if (defensesUpTimer <= 0)
	{
		defensesUpTimer = 0; // �h��̓A�b�v���Ԃł͂Ȃ�
		defenses = 0;        // �h��̓��Z�b�g
	}
}