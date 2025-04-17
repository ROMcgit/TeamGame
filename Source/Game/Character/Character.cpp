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
	hpDamageTimer = 0;

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
};

/*****************************************************************************************/

/*! �ړ��֘A */
#if 1

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
void Character::Move3D_Player(float vx, float vz, float speed, bool slide)
{
	// �ړ������x�N�g����ݒ�
	moveVecX = vx;
	moveVecZ = vz;

	//! �Q�[���p�b�h�̃C���X�^���X�擾
	GamePad& gamePad = Input::Instance().GetGamePad();

	// ���X�e�B�b�N�̓��͒l�擾
	float axisLX = gamePad.GetAxisLX();
	float axisLY = gamePad.GetAxisLY();

	// �X�e�B�b�N�̓��͂̋����i0.0�`1.0�j���v�Z
	float stickMagnitude = std::hypot(axisLX, axisLY);

	//! �X���C�h�X�e�[�g����Ȃ��Ȃ�
	if (!slide)
		// �ő呬�x�ݒ�
		maxMoveSpeed = abs(speed * stickMagnitude);
	//! �X���C�h�X�e�[�g�Ȃ�
	else
		// �ő呬�x�ݒ�
		maxMoveSpeed = speed;
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
void Character::Move2D_Player(float vx, float speed, bool slide)
{
	// �ړ������x�N�g����ݒ�
	moveVecX = vx;

	//! �Q�[���p�b�h�̃C���X�^���X�擾
	GamePad& gamePad = Input::Instance().GetGamePad();


	//! �X���C�h�X�e�[�g����Ȃ��Ȃ�
	if (!slide)
		// �ő呬�x�ݒ�
		maxMoveSpeed = abs(speed * gamePad.GetAxisLX());
	//! �X���C�h�X�e�[�g�Ȃ�
	else
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
void Character::Turn3D_Player(float elapsedTime, float vx, float vz, float speed)
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
void Character::Turn2D_Player()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//! �����͂����Ȃ�
	if ((gamePad.GetButtonHeld() & GamePad::BTN_LEFT) && !(gamePad.GetButtonDown() & GamePad::BTN_RIGHT))
	{
		//! �p�x�ύX������
		SetAngleChangeUnlock();

		//! �p�x�ύX����
		SetAngleYChange(DirectX::XMConvertToRadians(-90), 0.1f);
	}
	//! �E���͂����Ȃ�
	else if ((gamePad.GetButtonHeld() & GamePad::BTN_RIGHT) && !(gamePad.GetButtonDown() & GamePad::BTN_LEFT))
	{
		//! �p�x�ύX������
		SetAngleChangeUnlock();

		//! �p�x��ύX����
		SetAngleYChange(DirectX::XMConvertToRadians(90), 0.1f);
	}
}

//! �W�����v����
void Character::Jump(float speed)
{
	// ������̗͂�ݒ�
	velocity.y = speed;
}

#endif

/*****************************************************************************************************************/

/*! �G�֘A */
#if 1

// �^�[�Q�b�g�ɓ����������𔻒�
bool Character::IsArrivalTarget()
{
	// �ڕW�n�_�܂�XZ���ʂł̋�������
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;
	if (distSq < (radius * radius) * 1.5f) // �o�O�h�~�̂��߁A�����傫������
		return true;

	return false;
}

// �^�[�Q�b�g�̕�������
void Character::DirectionToTarget(float elapsedTime, float speedRate)
{
	// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// �����̏���
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// �^�[�Q�b�g�̕��ֈ꒼���ɐi��
void Character::MoveToDirection_3D(float speedRate)
{
	// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// �ړ�����
	Move3D(vx, vz, moveSpeed * speedRate);
}

// �G�̔j������
void Character::Destroy_Enemy()
{
	auto& manager = EnemyManager::Instance();
	for (auto& enemy : manager.enemies)
	{
		if (enemy.get() == this) // `this` �|�C���^�ɑΉ����� `shared_ptr` ��T��
		{
			manager.Remove(enemy.get());
			break;
		}
	}
}

#endif