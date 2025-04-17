#include "GameObjectBase.h"
#include "Game/Stage/StageManager.h"

#include "Game/Camera/Camera.h"
#include "Game/Camera/CameraController.h"

#include "Input/Input.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Other/Easing.h"
#include <imgui.h>
#include <random>

// ���S�������ɌĂ΂��
void GameObjectBase::OnDead()
{
}

/*****************************************************************************************/

/*! HP */
#if 1

//! HP�Ǘ�
void GameObjectBase::HpControll(float elapsedTime)
{
	// HP������𒴂��Ȃ��悤��
	if (hp > maxHp)       hp = maxHp;
	if (hpDamage > maxHp) hpDamage = maxHp;

	// �s���`�Ȃ�F��ς���(�v���C���[)
	//if (hp < maxHp * 0.25f) hpSpriteColorP = { 1.0f, 0.4118f, 0.7059f };

	// �_���[�W����
	if (hpDamage > hp)
	{
		hpDamageTimer += elapsedTime;
		if (hpDamageTimer >= 0.8f)
		{
			hpDamageDirectorWaitTimer -= elapsedTime;
			if (hpDamageDirectorWaitTimer <= 0)
			{
				hpDamage--;
				hpDamageDirectorWaitTimer = maxhpDamageDirectorWaitTimer;
			}
		}
	}
	// �񕜂�����
	else if (hpDamage < hp) hpDamage = hp;

	// HP�Ȃǂ�0��艺�ɂȂ�Ȃ��悤�ɂ���
	if (hp < 0) hp = 0;

	if (hpDamage == hp) hpDamageTimer = 0;
}

//! HP���o
bool GameObjectBase::HpDirector(float elapsedTime)
{
	//! �ŏ�����HP���ő�Ȃ�
	if (hp == maxHp)
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
			sound.PlaySound("HP���o", 0.5f);

		//---------------------------------------------------------------------//

		//! ���G�ɂ���
		invincibleTimer = 0.1f;

		//! �o�ߎ��Ԃ��v��
		hpDirectorElapsedTime += elapsedTime;

		float t = hpDirectorElapsedTime / 0.8f;

		//! HP����
		hp = Easing::Linear(0, maxHp, t);

		//! t��1���傫���Ȃ�
		if (t >= 1.0f)
		{
			//! ���ʉ����~�߂�
			sound.StopSound("HP���o");
			hpDirectorFinished = true;
		}
	}

	return true;
}

//! HP�s���`�X�V����(�G�̂�)
bool GameObjectBase::UpdateHpPinchEnemy()
{
	//! HP��0�Ȃ珈�����~�߂�
	if (hp <= 0 && hpDirectorFinished)
	{
		//! �G�~�V�b�u�̋�����0�ɂ���
		emissiveStrength = 0.0f;
		return false;
	}

	//! HP��30%���傫���Ȃ�A�������~�߂�
	if (hp > maxHp * 0.3f)
		return false;

	//! HP��25%���傫���Ȃ�
	if (hp > maxHp * 0.25f)
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.1f, 0.5f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.5f);
	}
	//! HP��20%���傫���Ȃ�
	else if (hp > maxHp * 0.2f)
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.2f, 0.4f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.4f);
	}
	//! HP��9%���傫���Ȃ�
	else if (hp > maxHp * 0.09f)
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.3f, 0.3f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.3f);
	}
	else
	{
		//! �G�~�b�V�u�̋�����0�Ȃ�
		if (emissiveStrength == 0.0f && !emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.7f, 0.2f);
		else if (!emissiveStrengthChange)
			// �G�~�b�V�u�̋�����ύX
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.2f);
	}

	return true;
}

//! HP�V�F�C�N
bool GameObjectBase::UpdateHpShake(float elapsedTime)
{
	// �J�����V�F�C�N�^�C�}�[��0.15f�ȉ��Ȃ�
	if (hpShakeTimer <= 0.15f && hpShake)
	{
		hpSpritePos.y = hpSpriteShakePosY - 4.5f; // HP�Q�[�W�̃V�F�C�N�ʒu
		hpSpriteColor = { 1, 0.6f, 1 };           // HP�Q�[�W�̐F


		hpImagePos.y = hpImageShakePosY - 4.5f;  // HP�摜�̃V�F�C�N�ʒu
		hpImageColor = { 1, 0, 0 };              // HP�摜�̐F

		hpSpriteColorP = { 1, 0, 0 };             // HP�Q�[�W��ԐF�ɂ���(�v���C���[)

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
bool GameObjectBase::UpdateHpPhaseChange(float elapsedTime)
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
void GameObjectBase::UpdateHpColorChange(float& hpImageColor, float& hpImageColorChangeSpeed, bool& hpImageColorUp, float elapsedTime)
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
void GameObjectBase::UpdateHpSpriteChange(float& hpSpriteColor, float& hpSpriteColorChangeSpeed, bool& hpSpriteColorDown, float elapsedTime)
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

#endif

/*****************************************************************************************************************/
/*! �Z�b�^�[ */
#if 1

/*! �J���� */
#if 1

//! �����̐ݒ�
void GameObjectBase::SetDirection(const DirectX::XMFLOAT3& direction)
{
	DirectX::XMFLOAT3 dir = direction;

	DirectX::XMVECTOR DIR;
	DIR = DirectX::XMLoadFloat3(&dir);
	DIR = DirectX::XMVector3Normalize(DIR);
	DirectX::XMStoreFloat3(&dir, DIR);

	this->direction = dir;
}

// �J�����̃^�[�Q�b�g�̈ʒu�̐ݒ�
void GameObjectBase::SetCameraTargetChange(DirectX::XMFLOAT3 endCameraTarget, float cameraTargetChangeTime, CameraTargetChangeEasing cameraTargetChangeEasing)
{
	if (!CameraController::targetChange &&
		(CameraController::target.x != endCameraTarget.x
			|| CameraController::target.y != endCameraTarget.y
			|| CameraController::target.z != endCameraTarget.z))
	{
		CameraController::targetChange = true;                                                                        // �J�����̃^�[�Q�b�g�̈ʒu��ς���
		CameraController::startTargetChange = CameraController::target;											         // �J�����̃^�[�Q�b�g�̕ύX�̊J�n�̒l
		CameraController::endTargetChange = endCameraTarget;                                                             // �����܂Ń^�[�Q�b�g�̈ʒu��ς���
		CameraController::targetChangeTime = cameraTargetChangeTime;                                                      // �^�[�Q�b�g�̈ʒu��ς��鎞��
		CameraController::targetChangeEasing = static_cast<CameraController::TargetChangeEasing>(cameraTargetChangeEasing); // �ǂ̃C�[�W���O�ɂ��邩
		CameraController::targetChangeElapsedTime = 0.0f;                                                                        // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (CameraController::targetChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �J�����̃^�[�Q�b�g�̈ʒu�̃��Z�b�g�̐ݒ�
void GameObjectBase::SetCameraTargetResetChange(float cameraTargetResetChangeTime, CameraTargetChangeEasing cameraTargetChangeEasing)
{
	//! �J�����̈ʒu�̕ύX������
	CameraController::targetChange = false;

	if ((CameraController::target.x != CameraController::targetReset.x
		|| CameraController::target.y != CameraController::targetReset.y
		|| CameraController::target.z != CameraController::targetReset.z))
	{
		CameraController::targetChange = true;                                                                        // �J�����̃^�[�Q�b�g�̈ʒu��ς���
		CameraController::startTargetChange = CameraController::target;											         // �J�����̃^�[�Q�b�g�̕ύX�̊J�n�̒l
		CameraController::endTargetChange = CameraController::targetReset;                                               // �����܂Ń^�[�Q�b�g�̈ʒu��ς���
		CameraController::targetChangeTime = cameraTargetResetChangeTime;                                                 // �^�[�Q�b�g�̈ʒu��ς��鎞��
		CameraController::targetChangeEasing = static_cast<CameraController::TargetChangeEasing>(cameraTargetChangeEasing); // �ǂ̃C�[�W���O�ɂ��邩
		CameraController::targetChangeElapsedTime = 0.0f;                                                                        // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (CameraController::targetChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �J�����̊p�x�ύX�̐ݒ�
void GameObjectBase::SetCameraAngleChange(DirectX::XMFLOAT3 endCameraAngle, float cameraAngleChangeTime, CameraAngleChangeEasing cameraAngleChangeEasing)
{
	if ((CameraController::angle.x != endCameraAngle.x)
		|| CameraController::angle.y != endCameraAngle.y
		|| CameraController::angle.z != endAngleChange.z)
	{
		CameraController::angleChange = true;                                                                      // �J�����̊p�x��ς���
		CameraController::startAngleChange = CameraController::angle;											          // �J�����̊p�x�̕ύX�̊J�n�̒l
		CameraController::endAngleChange = endCameraAngle;                                                            // �����܂Ŋp�x��ς���
		CameraController::angleChangeTime = cameraAngleChangeTime;                                                     // �p�x��ς��鎞��
		CameraController::angleChangeEasing = static_cast<CameraController::AngleChangeEasing>(cameraAngleChangeEasing); // �ǂ̃C�[�W���O�ɂ��邩
		CameraController::angleChangeElapsedTime = 0.0f;                                                                      // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (CameraController::angleChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �J�����̊p�x�̃��Z�b�g�̐ݒ�
void GameObjectBase::SetCameraAngleResetChange(float cameraAngleResetChangeTime, CameraAngleChangeEasing cameraAngleChangeEasing)
{
	//! �J�����̊p�x�̕ύX������
	CameraController::angleChange = false;

	if ((CameraController::angle.x != CameraController::angleReset.x)
		|| CameraController::angle.y != CameraController::angleReset.y
		|| CameraController::angle.z != CameraController::angleReset.z)
	{
		CameraController::angleChange = true;                                                                      // �J�����̊p�x��ς���
		CameraController::startAngleChange = CameraController::angle;											          // �J�����̊p�x�̕ύX�̊J�n�̒l
		CameraController::endAngleChange = CameraController::angleReset;                                              // �����܂Ŋp�x��ς���
		CameraController::angleChangeTime = cameraAngleResetChangeTime;                                                // �p�x��ς��鎞��
		CameraController::angleChangeEasing = static_cast<CameraController::AngleChangeEasing>(cameraAngleChangeEasing); // �ǂ̃C�[�W���O�ɂ��邩
		CameraController::angleChangeElapsedTime = 0.0f;                                                                      // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (CameraController::angleChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �J�����͈̔͂̕ύX�̐ݒ�
void GameObjectBase::SetCameraRangeChange(float endCameraRange, float cameraRangeChangeTime, CameraRangeChangeEasing cameraRangeChangeEasing)
{
	if (!CameraController::rangeChange && CameraController::range != endCameraRange)
	{
		CameraController::rangeChange = true;                                                                      // �J�����͈̔͂�ς���
		CameraController::startRangeChange = CameraController::range;											          // �J�����͈̔͂̕ύX�̊J�n�̒l
		CameraController::endRangeChange = endCameraRange;                                                            // �����܂ŃJ�����͈̔͂�ς���
		CameraController::rangeChangeTime = cameraRangeChangeTime;                                                     // �J�����͈̔͂�ς��鎞��
		CameraController::rangeChangeEasing = static_cast<CameraController::RangeChangeEasing>(cameraRangeChangeEasing); // �ǂ̃C�[�W���O�ɂ��邩
		CameraController::rangeChangeElapsedTime = 0.0f;                                                                      // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (CameraController::rangeChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �J�����͈̔͂̃��Z�b�g�̐��l
void GameObjectBase::SetCameraRangeResetChange(float cameraRangeResetChangeTime, CameraRangeChangeEasing cameraRangeChangeEasing)
{
	//! �J�����͈̔͂̕ύX������
	CameraController::rangeChange = false;

	if (CameraController::range != CameraController::rangeReset)
	{
		CameraController::rangeChange = true;                                                                      // �J�����͈̔͂�ς���
		CameraController::startRangeChange = CameraController::range;											          // �J�����͈̔͂̕ύX�̊J�n�̒l
		CameraController::endRangeChange = CameraController::rangeReset;                                              // �����܂ŃJ�����͈̔͂�ς���
		CameraController::rangeChangeTime = cameraRangeResetChangeTime;                                                // �J�����͈̔͂�ς��鎞��
		CameraController::rangeChangeEasing = static_cast<CameraController::RangeChangeEasing>(cameraRangeChangeEasing); // �ǂ̃C�[�W���O�ɂ��邩
		CameraController::rangeChangeElapsedTime = 0.0f;                                                                      // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (CameraController::rangeChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �J�����V�F�C�N�ݒ�
void GameObjectBase::SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower)
{
	CameraController::cameraShake = true;             // �J�����V�F�C�N���邩
	CameraController::cameraShakeTimer = cameraShakeTimer; // �J�����V�F�C�N���s������
	CameraController::cameraShakePower = cameraShakePower; // �J�����V�F�C�N�̋���
}

#endif

/*****************************************************************************************************************/

/*! �|�X�g�G�t�F�N�g */
#if 1

//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
void GameObjectBase::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3& ColorFilter, float chromaticAberration)
{
	Camera::postEffect.contrast = contrast;            // �R���X�g���N�g
	Camera::postEffect.saturation = saturation;		  // �T�`�����[�V����
	Camera::postEffect.colorFilter = ColorFilter;		  // �J���[�t�B���^�[
	Camera::postEffect.chromaticAberration = chromaticAberration; // �N���}�e�B�b�N�A�x���[�V����
}

//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����Z�b�g����
void GameObjectBase::SetPostEffectStatusResetChange(
	float contrastChangeTime,
	float saturationChangeTime,
	float colorFilterChangeTime,
	float chromaticAberrationChangeTime,
	float blurStrengthChangeTime)
{
	//! �ύX��������U�~�߂�
	Camera::contrastChange = false; // �R���g���X�g
	Camera::saturationChange = false; // �T�`�����[�V����
	Camera::colorFilterChange = false; // �J���[�t�B���^�[
	Camera::chromaticAberrationChange = false; // �N���}�e�B�b�N�A�x���[�V����
	Camera::blurStrengthChange = false; // �u���[�̋��x

	//! �R���g���X�g
	Camera::contrastChange = true;                        // �R���g���X�g�̒l��ς���
	Camera::startContrastChange = Camera::postEffect.contrast; // �R���g���X�g�̕ύX�̊J�n�̒l
	Camera::endContrastChange = Camera::contrastReset;       // �����܂ŃR���g���X�g�̒l��ς���
	Camera::contrastChangeTime = contrastChangeTime;          // �R���g���X�g�̒l��ς��鎞��
	Camera::contrastChangeElapsedTime = 0.0f;                        // �o�ߎ��Ԃ����Z�b�g

	//! �T�`�����[�V����
	Camera::saturationChange = true;                          // �T�`�����[�V�����̒l��ς���
	Camera::startSaturationChange = Camera::postEffect.saturation; // �T�`�����[�V�����̕ύX�̊J�n�̒l
	Camera::endSaturationChange = Camera::saturationReset;       // �����܂ŃT�`�����[�V�����̒l��ς���
	Camera::saturationChangeTime = saturationChangeTime;          // �T�`�����[�V�����̒l��ς��鎞��
	Camera::saturationChangeElapsedTime = 0.0f;                          // �o�ߎ��Ԃ����Z�b�g

	//! �J���[�t�B���^�[�̒l��ύX���Ă��Ȃ��Ȃ�
	Camera::colorFilterChange = true;                           // �J���[�t�B���^�[�̒l��ς���
	Camera::startColorFilterChange = Camera::postEffect.colorFilter; // �J���[�t�B���^�[�̕ύX�̊J�n�̒l
	Camera::endColorFilterChange = Camera::colorFilterReset;       // �����܂ŃJ���[�t�B���^�[�̒l��ς���
	Camera::colorFilterChangeTime = colorFilterChangeTime;          // �J���[�t�B���^�[�̒l��ς��鎞��
	Camera::colorFilterChangeElapsedTime = 0.0f;                           // �o�ߎ��Ԃ����Z�b�g

	//! �N���}�e�B�b�N�A�x���[�V����
	Camera::chromaticAberrationChange = true;                                   // �N���}�e�B�b�N�A�x���[�V�����̒l��ς���
	Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // �N���}�e�B�b�N�A�x���[�V�����̕ύX�̊J�n�̒l
	Camera::endChromaticAberrationChange = Camera::chromaticAberrationReset;       // �����܂ŃN���}�e�B�b�N�A�x���[�V�����̒l��ς���
	Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鎞��
	Camera::chromaticAberrationChangeElapsedTime = 0.0f;                                   // �o�ߎ��Ԃ����Z�b�g

	//! �u���[�̋��x
	Camera::blurStrengthChange = true;                            // �u���[�̋��x��ς���
	Camera::startBlurStrengthChange = Camera::postEffect.blurStrength; // �u���[�̋��x�̕ύX�̊J�n�̒l
	Camera::endBlurStrengthChange = Camera::blurStrengthReset;       // �����܂Ńu���[�̋��x��ς���
	Camera::blurStrengthChangeTime = blurStrengthChangeTime;          // �u���[�̋��x��ς��鎞��
	Camera::blurStrengthChangeElapsedTime = 0.0f;                            // �o�ߎ��Ԃ����Z�b�g
}

/*! �R���g���X�g */
#if 1

// �R���g���X�g�̐ݒ�
void GameObjectBase::SetContrast(float contrast)
{
	Camera::postEffect.contrast = contrast;
}

// �R���g���X�g�ύX�̐ݒ�
void GameObjectBase::SetContrastChange(float endContrastChange, float contrastChangeTime)
{
	if (!Camera::contrastChange && Camera::postEffect.contrast != endContrastChange)
	{
		Camera::contrastChange = true;                        // �R���g���X�g�̒l��ς���
		Camera::startContrastChange = Camera::postEffect.contrast; // �R���g���X�g�̕ύX�̊J�n�̒l
		Camera::endContrastChange = endContrastChange;           // �����܂ŃR���g���X�g�̒l��ς���
		Camera::contrastChangeTime = contrastChangeTime;          // �R���g���X�g�̒l��ς��鎞��
		Camera::contrastChangeElapsedTime = 0.0f;                        // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::contrastChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �R���g���X�g�̃��Z�b�g
void GameObjectBase::SetContrastReset()
{
	Camera::postEffect.contrast = Camera::contrastReset;
}

// �R���g���X�g���Z�b�g�ύX�̐ݒ�
void GameObjectBase::SetContrastResetChange(float contrastChangeTime)
{
	//! �R���g���X�g�̕ύX������
	Camera::contrastChange = false;

	if (!Camera::contrastChange)
	{
		Camera::contrastChange = true;                        // �R���g���X�g�̒l��ς���
		Camera::startContrastChange = Camera::postEffect.contrast; // �R���g���X�g�̕ύX�̊J�n�̒l
		Camera::endContrastChange = Camera::contrastReset;       // �����܂ŃR���g���X�g�̒l��ς���
		Camera::contrastChangeTime = contrastChangeTime;          // �R���g���X�g�̒l��ς��鎞��
		Camera::contrastChangeElapsedTime = 0.0f;                        // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::contrastChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! �T�`�����[�V���� */
#if 1

// �T�`�����[�V����(�F�̍ʓx)�̐ݒ�
void GameObjectBase::SetSaturation(float saturation)
{
	Camera::postEffect.saturation = saturation;
}

// �T�`�����[�V����(�F�̍ʓx)�ύX�̐ݒ�
void GameObjectBase::SetSaturationChange(float endSaturationChange, float saturationChangeTime)
{
	if (!Camera::saturationChange && Camera::postEffect.saturation != endSaturationChange)
	{
		Camera::saturationChange = true;                          // �T�`�����[�V�����̒l��ς���
		Camera::startSaturationChange = Camera::postEffect.saturation; // �T�`�����[�V�����̕ύX�̊J�n�̒l
		Camera::endSaturationChange = endSaturationChange;           // �����܂ŃT�`�����[�V�����̒l��ς���
		Camera::saturationChangeTime = saturationChangeTime;          // �T�`�����[�V�����̒l��ς��鎞��
		Camera::saturationChangeElapsedTime = 0.0f;                          // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::saturationChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �T�`�����[�V�����̃��Z�b�g
void GameObjectBase::SetSaturationReset()
{
	Camera::postEffect.saturation = Camera::saturationReset;
}

// �T�`�����[�V�������Z�b�g�̕ύX�̐ݒ�
void GameObjectBase::SetSaturationResetChange(float saturationChangeTime)
{
	//! �T�`�����[�V�����̕ύX�̉���
	Camera::saturationChange = false;

	if (!Camera::saturationChange)
	{
		Camera::saturationChange = true;                          // �T�`�����[�V�����̒l��ς���
		Camera::startSaturationChange = Camera::postEffect.saturation; // �T�`�����[�V�����̕ύX�̊J�n�̒l
		Camera::endSaturationChange = Camera::saturationReset;       // �����܂ŃT�`�����[�V�����̒l��ς���
		Camera::saturationChangeTime = saturationChangeTime;          // �T�`�����[�V�����̒l��ς��鎞��
		Camera::saturationChangeElapsedTime = 0.0f;                          // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::saturationChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! �J���[�t�B���^�[ */
#if 1

// �J���[�t�B���^�[�̐ݒ�
void GameObjectBase::SetColorFilter(const DirectX::XMFLOAT3& colorFilter)
{
	Camera::postEffect.colorFilter = colorFilter;
}

// �J���[�t�B���^�[�ύX�̐ݒ�
void GameObjectBase::SetColorFilterChange(DirectX::XMFLOAT3& endColorFilterChange, float colorFilterChangeTime)
{
	if (!Camera::colorFilterChange &&
		(Camera::postEffect.colorFilter.x != endColorFilterChange.x
			|| Camera::postEffect.colorFilter.y != endColorFilterChange.y
			|| Camera::postEffect.colorFilter.z != endColorFilterChange.z))
	{
		Camera::colorFilterChange = true;                           // �J���[�t�B���^�[�̒l��ς���
		Camera::startColorFilterChange = Camera::postEffect.colorFilter; // �J���[�t�B���^�[�̕ύX�̊J�n�̒l
		Camera::endColorFilterChange = endColorFilterChange;           // �����܂ŃJ���[�t�B���^�[�̒l��ς���
		Camera::colorFilterChangeTime = colorFilterChangeTime;          // �J���[�t�B���^�[�̒l��ς��鎞��
		Camera::colorFilterChangeElapsedTime = 0.0f;                           // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::colorFilterChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �J���[�t�B���^�[�̃��Z�b�g
void GameObjectBase::SetColorFilterReset()
{
	Camera::postEffect.colorFilter = Camera::colorFilterReset;
}

// �J���[�t�B���^�[���Z�b�g�̕ύX�̐ݒ�
void GameObjectBase::SetColorFilterResetChange(float colorFilterChangeTime)
{
	//! �J���[�t�B���^�[�̕ύX������
	Camera::colorFilterChange = false;

	if (!Camera::colorFilterChange)
	{
		Camera::colorFilterChange = true;                           // �J���[�t�B���^�[�̒l��ς���
		Camera::startColorFilterChange = Camera::postEffect.colorFilter; // �J���[�t�B���^�[�̕ύX�̊J�n�̒l
		Camera::endColorFilterChange = Camera::colorFilterReset;       // �����܂ŃJ���[�t�B���^�[�̒l��ς���
		Camera::colorFilterChangeTime = colorFilterChangeTime;          // �J���[�t�B���^�[�̒l��ς��鎞��
		Camera::colorFilterChangeElapsedTime = 0.0f;                           // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::colorFilterChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! �N���}�e�B�b�N�A�x���[�V���� */
#if 1

// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�̐ݒ�
void GameObjectBase::SetChromaticAberration(float chromaticAberration)
{
	Camera::postEffect.chromaticAberration = chromaticAberration;
}

// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�ύX�̐ݒ�
void GameObjectBase::SetChromaticAberrationChange(float endChromaticAberrationChange, float chromaticAberrationChangeTime)
{
	if (!Camera::chromaticAberrationChange && Camera::postEffect.chromaticAberration != endChromaticAberrationChange)
	{
		Camera::chromaticAberrationChange = true;                                   // �N���}�e�B�b�N�A�x���[�V�����̒l��ς���
		Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // �N���}�e�B�b�N�A�x���[�V�����̕ύX�̊J�n�̒l
		Camera::endChromaticAberrationChange = endChromaticAberrationChange;           // �����܂ŃN���}�e�B�b�N�A�x���[�V�����̒l��ς���
		Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鎞��
		Camera::chromaticAberrationChangeElapsedTime = 0.0f;                                   // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::chromaticAberrationChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �N���}�e�B�b�N�A�x���[�V�����̃��Z�b�g
void GameObjectBase::SetChromaticAberrationReset()
{
	Camera::postEffect.chromaticAberration = Camera::chromaticAberrationReset;
}

// �N���}�e�B�b�N�A�x���[�V�������Z�b�g�̐ݒ�
void GameObjectBase::SetChromaticAberrationResetChange(float chromaticAberrationChangeTime)
{
	//! �N���}�e�B�b�N�A�x���[�V�����̕ύX������
	Camera::chromaticAberrationChange = false;

	if (!Camera::chromaticAberrationChange)
	{
		Camera::chromaticAberrationChange = true;                                   // �N���}�e�B�b�N�A�x���[�V�����̒l��ς���
		Camera::startChromaticAberrationChange = Camera::postEffect.chromaticAberration; // �N���}�e�B�b�N�A�x���[�V�����̕ύX�̊J�n�̒l
		Camera::endChromaticAberrationChange = Camera::chromaticAberrationReset;       // �����܂ŃN���}�e�B�b�N�A�x���[�V�����̒l��ς���
		Camera::chromaticAberrationChangeTime = chromaticAberrationChangeTime;          // �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鎞��
		Camera::chromaticAberrationChangeElapsedTime = 0.0f;                                   // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::chromaticAberrationChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

#endif

//----------------------------------------------------------------------------------------------------------------//

/*! �u���[�̋��x */
#if 1

// �u���[�̋��x��ݒ�
void GameObjectBase::SetBlurStrength(float blurStrength)
{
	Camera::postEffect.blurStrength = blurStrength;
}

// �u���[�̋��x�ύX�̐ݒ�
void GameObjectBase::SetBlurStrengthChange(float endBlurStrengthChange, float blurStrengthChangeTime)
{
	if (!Camera::blurStrengthChange && Camera::postEffect.blurStrength != endBlurStrengthChange)
	{
		Camera::blurStrengthChange = true;                            // �u���[�̋��x��ς���
		Camera::startBlurStrengthChange = Camera::postEffect.blurStrength; // �u���[�̋��x�̕ύX�̊J�n�̒l
		Camera::endBlurStrengthChange = endBlurStrengthChange;           // �����܂Ńu���[�̋��x��ς���
		Camera::blurStrengthChangeTime = blurStrengthChangeTime;          // �u���[�̋��x��ς��鎞��
		Camera::blurStrengthChangeElapsedTime = 0.0f;                            // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::blurStrengthChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

// �u���[�̋��x�̃��Z�b�g
void GameObjectBase::SetBlurStrengthReset()
{
	Camera::postEffect.blurStrength = Camera::blurStrengthReset;
}

// �u���[�̋��x���Z�b�g�̐ݒ�
void GameObjectBase::SetBlurStrengthResetChange(float blurStrengthChangeTime)
{
	//! �u���[�̋��x�̕ύX������
	Camera::blurStrengthChange = false;

	if (!Camera::blurStrengthChange)
	{
		Camera::blurStrengthChange = true;                            // �u���[�̋��x��ς���
		Camera::startBlurStrengthChange = Camera::postEffect.blurStrength; // �u���[�̋��x�̕ύX�̊J�n�̒l
		Camera::endBlurStrengthChange = Camera::blurStrengthReset;       // �����܂Ńu���[�̋��x��ς���
		Camera::blurStrengthChangeTime = blurStrengthChangeTime;          // �u���[�̋��x��ς��鎞��
		Camera::blurStrengthChangeElapsedTime = 0.0f;                            // �o�ߎ��Ԃ����Z�b�g

		// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
		if (Camera::blurStrengthChangeTime <= 0)
		{
			throw std::out_of_range("�C�[�W���O�o���܂���I�I");
		}
	}
}

#endif

// �J�����V�F�C�N(�V�F�[�_�[)�̐ݒ�
void GameObjectBase::SetCameraShakeShader(float cameraShakeTime, float cameraShakeStrength, DirectX::XMFLOAT2 cameraShakeOffset, float cameraShakeSpeed)
{
	if (!Camera::cameraShakeShader)
	{
		Camera::cameraShakeShader = true;                // �J�����V�F�C�N(�V�F�[�_�[)������
		Camera::cameraShakeShaderTime = cameraShakeTime;     // �J�����V�F�C�N(�V�F�[�_�[)�̎���
		Camera::postEffect.shakeStrength = cameraShakeStrength; // �J�����V�F�C�N(�V�F�[�_�[)�̋���
		Camera::postEffect.shakeOffset = cameraShakeOffset;   // �J�����V�F�C�N(�V�F�[�_�[)�̂��炷�ʒu
		Camera::postEffect.shakeSpeed = cameraShakeSpeed;    // �J�����V�F�C�N(�V�F�[�_�[)�̑���
	}
}

// �J�����V�F�C�N(�V�F�[�_�[)�̉���
void GameObjectBase::SetCameraShakeShaderUnlock()
{
	Camera::cameraShakeShader = false; // �J�����V�F�C�N(�V�F�[�_�[)������
	Camera::postEffect.shakeTime = 0.0f;  // �J�����V�F�C�N�̌v�����Ԃ�0�ɂ���
	Camera::postEffect.shakeStrength = 0.0f;  // �J�����V�F�C�N�̋�����0�ɂ���
}

//----------------------------------------------------------------------------------------------------------------//

/*! �Q�b�^�[ */
#if 1

// �R���g���X�g��ύX���Ă��邩�擾
bool GameObjectBase::GetContrastChange()
{
	return Camera::contrastChange;
}

// �T�`�����[�V����(�F�̍ʓx)��ύX���Ă��邩���擾
bool GameObjectBase::GetSaturationChange()
{
	return Camera::saturationChange;
}

// �J���[�t�B���^�[��ύX���Ă��邩���擾
bool GameObjectBase::GetColorFilterChange()
{
	return Camera::colorFilterChange;
}

// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))��ύX���Ă��邩���擾
bool GameObjectBase::GetChromaticAberrationChange()
{
	return Camera::chromaticAberrationChange;
}

// �u���[�̋��x��ύX���Ă��邩�擾
bool GameObjectBase::GetBlurStrengthChange()
{
	return Camera::blurStrengthChange;
}

#endif // �Q�b�^�[

#endif // �|�X�g�G�t�F�N�g

#endif // �Z�b�^�[

/*****************************************************************************************************************/

//! ��ԍX�V����
void GameObjectBase::UpdateGameObjectBaseState(float elapsedTime, Object object)
{
	// �q�b�g�X�g�b�v�X�V����
	UpdateHitStop(elapsedTime);

	switch (object)
	{
	//! �L�����N�^�[
	case Object::Character:
	{
		//! �s��X�V����
		UpdateTransform();

		//! ���͍X�V����
		UpdateVelocity(hitStopElapsedTime);
	}
	break;
	//! �X�e�[�W
	case Object::Projectile:
	{
		//! �s��X�V����(��ѓ���)
		UpdateTransform_Projectile();

		//! ���C�L���X�g�X�V����(��ѓ���)
		UpdateProjectileRaycast();
	}
	break;
	//! ��ѓ���
	case Object::Stage:
	{
		//! �s��X�V����(�X�e�[�W)
		UpdateTransform_Stage();
	}
	break;
	default:
		break;
	}

	/*! HP */
#if 1

	// HP�Ǘ�
	HpControll(elapsedTime);

	// HP�V�F�C�N	�X�V����
	UpdateHpShake(elapsedTime);

	// HP�̌`�ԕω����o
	UpdateHpPhaseChange(elapsedTime);

#endif

//---------------------------------------------------------//

	/*! �ʒu */
#if 1

	// �ʒu�ύX�X�V����
	UpdatePositionAllChange(hitStopElapsedTime);

	// �ʒuX�ύX�X�V����
	UpdatePositionXChange(hitStopElapsedTime);

	// �ʒuY�ύX�X�V����
	UpdatePositionYChange(hitStopElapsedTime);

	// �ʒuZ�ύX�X�V����
	UpdatePositionZChange(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	/*! �p�x */
#if 1

	// �p�x��������
	LimitAngle();

	// �p�x�S�̕ύX�X�V����
	UpdateAngleChange(hitStopElapsedTime);

	// �p�xX�ύX�X�V����
	UpdateAngleXChange(hitStopElapsedTime);

	// �p�xY�ύX�X�V����
	UpdateAngleYChange(hitStopElapsedTime);

	// �p�xZ�ύX�X�V����
	UpdateAngleZChange(hitStopElapsedTime);

	// �p�x�̉�]�̍X�V����(����̒l�͖ڎw�����A�w�肳�ꂽ���Ԓ��ɉ�]����)
	UpdateAngleRotation(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	/*! �X�P�[�� */
#if 1

	// �X�P�[���ύX�X�V����
	UpdateScaleChange(hitStopElapsedTime);

	// �X�P�[��X�ύX�X�V����
	UpdateScaleXChange(hitStopElapsedTime);

	// �X�P�[��Y�ύX�X�V����
	UpdateScaleYChange(hitStopElapsedTime);

	// �X�P�[��Z�ύX�X�V����
	UpdateScaleZChange(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	/*! �`��֘A */
#if 1

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

#endif

//---------------------------------------------------------//

	/*! �����蔻�� */
#if 1

	// �����蔻��̈ʒu��ݒ�
	CollisionPosSettings();

	// �����蔻��̃I�t�Z�b�g�ύX�X�V����
	UpdateCollisionOffsetChange(hitStopElapsedTime);

	// ���a�ύX�X�V����
	UpdateRadiusChange(hitStopElapsedTime);

	// �����ύX�X�V����
	UpdateHeightChange(hitStopElapsedTime);

	// ���ύX�X�V����
	UpdateWidthChange(hitStopElapsedTime);

#endif

//---------------------------------------------------------//

	// ���[�r�[���ԍX�V����
	UpdateMovieTimer(elapsedTime);

	// ���G���ԍX�V����
	UpdateInvincibleTimer(elapsedTime);

	// �h��̓A�b�v���ԍX�V
	UpdateDefensesUpTimer(elapsedTime);
}

//! �s��X�V����
void GameObjectBase::UpdateTransform()
{
	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	// �R�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;

	// �v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

// �s��X�V����(�X�e�[�W)
void GameObjectBase::UpdateTransform_Stage()
{
	// �ȑO�̕ϊ��s���ۑ�
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}

//! �s��X�V����(��ѓ���)
void GameObjectBase::UpdateTransform_Projectile()
{
	DirectX::XMVECTOR Front, Up, Right;

	// �O�x�N�g�����Z�o
	Front = DirectX::XMLoadFloat3(&direction);


	// ���̏�x�N�g�����Z�o
	Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	// �E�x�N�g�����Z�o
	Right = DirectX::XMVector3Cross(Up, Front);
	Right = DirectX::XMVector3Normalize(Right);


	// ��x�N�g�����Z�o
	Up = DirectX::XMVector3Cross(Front, Right);
	Up = DirectX::XMVector3Normalize(Up);


	// �v�Z���ʂ����o��
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	// �Z�o�������x�N�g������s����쐬
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0.0f;
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0.0f;
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0.0f;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

	// ���˕���
	this->direction = front;
}

//! ���C�L���X�g�X�V����
void GameObjectBase::UpdateProjectileRaycast()
{
	/*! �n�ʂƂ̃��C�L���X�g */
#if 1
	{
		// ��ѓ����Y�������ƂȂ�@���x�N�g��
		DirectX::XMFLOAT3 normal = { 0,1,0 };

		float collisionOffsetY = collisionOffset.y - (radius * 0.5f);

		// ���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x + collisionOffset.x, (position.y + 1.0f) + collisionOffsetY, position.z + collisionOffset.z };
		// ���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x + collisionOffset.x, (position.y) + collisionOffsetY, position.z + collisionOffset.z };

		// ���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �@���x�N�g���擾
			normal = hit.normal;

			isGround = true;
		}
		else
			isGround = false;
	}
#endif

	/*! �ǂƂ̃��C�L���X�g */
#if 1
	{
		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = {
			position.x + collisionOffset.x,
			position.y + collisionOffset.y + 0.01f,
			position.z + collisionOffset.z };

		//! �����蔻��
		float collisionX = this->radius * (direction.x > 0 ? 1 : -1);;
		float collisionZ = this->radius * (direction.z > 0 ? 1 : -1);;

		DirectX::XMFLOAT3 end = {
				(position.x + collisionOffset.x) + collisionX,
				(position.y + collisionOffset.y + 0.01f),
				(position.z + collisionOffset.z) + collisionZ
		};

		// ���C�L���X�g�ɂ��ǔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//! �ǂƐڐG����
			isWall = true;
		}
		else
		{
			//! �ǂƐڐG���Ă��Ȃ�
			isWall = false;
		}
	}
#endif
}

//! �q�b�g�X�g�b�v�X�V����
bool GameObjectBase::UpdateHitStop(float elapsedTime)
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

	//! �q�b�g�X�g�b�v���ɃJ������h�炷�Ȃ�
	if (hitStopCameraShake)
		//! �J�����������h�炷
		SetCameraShake(0.05f, DirectX::XMINT3(5, 8, 5));

	return true;
}

/********************************************************************************************/

/*! ���͏��� */
#if 1

//! �����x�X�V����
void GameObjectBase::UpdateVelocity(float elapsedTime)
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
void GameObjectBase::UpdateVerticalVelocity(float elapsedFrame)
{
	// TODO:�d�͏���
	velocity.y += -gravity * elapsedFrame;
}

// �����ړ��X�V����
void GameObjectBase::UpdateVerticalMove(float elapsedTime)
{
	// ���������̈ړ���
	float my = velocity.y * elapsedTime;

	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	DirectX::XMFLOAT3 normal = { 0,1,0 };

	// ������
	if (my < 0.0f)
	{
		//! �����蔻��̈ʒuY
		float collisionOffsetY = 0;
		switch (collisionType)
		{
			//! �{�b�N�X
		case CollisionType::Box:
			collisionOffsetY = collisionOffset.y - (height * 0.5f);
			break;
			//! ��
		case CollisionType::Sphere:
			collisionOffsetY = collisionOffset.y - (radius * 0.82f);
			break;
			//! �~��
		case CollisionType::Cylinder:
			collisionOffsetY = collisionOffset.y;
			break;
		default:
			break;
		}

		// ���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x + collisionOffset.x, (position.y + stepOffset) + collisionOffsetY, position.z + collisionOffset.z };
		// ���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x + collisionOffset.x, (position.y + my) + collisionOffsetY, position.z + collisionOffset.z };

		// ���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �@���x�N�g���擾
			normal = hit.normal;

			// �n�ʂɐڒn���Ă���
			position.x = hit.position.x - collisionOffset.x;
			position.y = hit.position.y - collisionOffsetY;
			position.z = hit.position.z - collisionOffset.z;

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
void GameObjectBase::UpdateHorizontalVelocity(float elapsedFrame)
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
void GameObjectBase::UpdateHorizontalMove(float elapsedTime)
{
	// �������͗ʌv�Z
	float velocityLengthXZ = sqrt((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (velocityLengthXZ > 0.0f)
	{
		// TODO:�����ړ��l
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = {
			position.x + collisionOffset.x,
			position.y + collisionOffset.y + 0.01f,
			position.z + collisionOffset.z };

		//! �����蔻��
		float collisionX = 0;
		float collisionZ = 0;

		//! �����蔻��̎�ނɂ���ĕς���
		switch (collisionType)
		{
		case CollisionType::Box:
		{
			collisionX = (this->width * 0.5f) * (mx > 0 ? 1 : -1);
			collisionZ = (this->depth * 0.5f) * (mz > 0 ? 1 : -1);
		}
		break;
		case CollisionType::Sphere:
		case CollisionType::Cylinder:
		{
			collisionX = this->radius * (mx > 0 ? 1 : -1);
			collisionZ = this->radius * (mz > 0 ? 1 : -1);
		}
		break;
		default:
			break;
		}

		DirectX::XMFLOAT3 end = {
			(position.x + collisionOffset.x) + collisionX,
			(position.y + collisionOffset.y + 0.01f),
			(position.z + collisionOffset.z) + collisionZ
		};

		// ���C�L���X�g�ɂ��ǔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//! �����x��0�ɂ���
			velocity.x = 0;
			velocity.z = 0;

			//! �ǂƐڐG����
			isWall = true;
		}
		else
		{
			//! �ړ�
			position.x += mx;
			position.z += mz;

			//! �ǂƐڐG���Ă��Ȃ�
			isWall = false;
		}
	}
}

#endif // ���͏���

/********************************************************************************************/

/*! �ʒu�ύX�X�V���� */
#if 1

//! �ʒu�S�̕ύX�X�V����
bool GameObjectBase::UpdatePositionAllChange(float elapsedTime)
{
	//! �ʒu��ς��Ȃ��Ȃ�A�������~�߂�
	if (!positionChange.all)
		return false;


	//! �o�ߎ��Ԃ��v��
	positionChangeElapsedTime.all += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = positionChangeElapsedTime.all / positionChangeTime.all;

#if 1
	switch (positionChangeEasing.all)
	{
		//! ���j�A(�⊮����)
	case PositionChangeEasing::Type::Linear:
	{
		//! �ʒu�ύX
		position.x = Easing::Linear(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::Linear(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::Linear(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseIn(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseIn(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseIn(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseOut(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseOut(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseInOut(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseInOut(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseInOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseBack(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseBack(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseBack(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseBounce(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseBounce(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseBounce(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseElastic(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseElastic(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseElastic(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseInCircle(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseInCircle(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseInCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseOutCircle(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseOutCircle(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseInOutCircle(startPositionChange.x, endPositionChange.x, t);
		position.y = Easing::EaseInOutCircle(startPositionChange.y, endPositionChange.y, t);
		position.z = Easing::EaseInOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		position = endPositionChange;
		positionChange.all = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------//

//! �ʒuX�ύX�X�V����
bool GameObjectBase::UpdatePositionXChange(float elapsedTime)
{
	//! �ʒu��ς��Ȃ��Ȃ�A�������~�߂�
	if (!positionChange.x)
		return false;


	//! �o�ߎ��Ԃ��v��
	positionChangeElapsedTime.x += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = positionChangeElapsedTime.x / positionChangeTime.x;

#if 1
	switch (positionChangeEasing.x)
	{
		//! ���j�A(�⊮����)
	case PositionChangeEasing::Type::Linear:
	{
		//! �ʒu�ύX
		position.x = Easing::Linear(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseIn(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseOut(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseInOut(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseBack(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseBounce(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseElastic(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseInCircle(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseOutCircle(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! �ʒu�ύX
		position.x = Easing::EaseInOutCircle(startPositionChange.x, endPositionChange.x, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		position.x = endPositionChange.x;
		positionChange.x = false;
	}

	return true;
}

//! �ʒuY�ύX�X�V����
bool GameObjectBase::UpdatePositionYChange(float elapsedTime)
{
	//! �ʒu��ς��Ȃ��Ȃ�A�������~�߂�
	if (!positionChange.y)
		return false;


	//! �o�ߎ��Ԃ��v��
	positionChangeElapsedTime.y += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = positionChangeElapsedTime.y / positionChangeTime.y;

#if 1
	switch (positionChangeEasing.y)
	{
		//! ���j�A(�⊮����)
	case PositionChangeEasing::Type::Linear:
	{
		//! �ʒu�ύX
		position.y = Easing::Linear(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseIn(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseOut(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseInOut(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseBack(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseBounce(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseElastic(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseInCircle(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseOutCircle(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! �ʒu�ύX
		position.y = Easing::EaseInOutCircle(startPositionChange.y, endPositionChange.y, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		position.y = endPositionChange.y;
		positionChange.y = false;
	}

	return true;
}

//! �ʒuZ�ύX�X�V����
bool GameObjectBase::UpdatePositionZChange(float elapsedTime)
{
	//! �ʒu��ς��Ȃ��Ȃ�A�������~�߂�
	if (!positionChange.z)
		return false;


	//! �o�ߎ��Ԃ��v��
	positionChangeElapsedTime.z += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = positionChangeElapsedTime.z / positionChangeTime.z;

#if 1
	switch (positionChangeEasing.z)
	{
		//! ���j�A(�⊮����)
	case PositionChangeEasing::Type::Linear:
	{
		//! �ʒu�ύX
		position.z = Easing::Linear(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case PositionChangeEasing::Type::EaseIn:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseIn(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case PositionChangeEasing::Type::EaseOut:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOut:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseInOut(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBack:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseBack(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseBounce:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseBounce(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseElastic:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseElastic(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInCircle:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseInCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseOutCircle:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	case PositionChangeEasing::Type::EaseInOutCircle:
	{
		//! �ʒu�ύX
		position.z = Easing::EaseInOutCircle(startPositionChange.z, endPositionChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		position.z = endPositionChange.z;
		positionChange.z = false;
	}

	return true;
}

#endif // �ʒu�ύX�X�V����

/********************************************************************************************/

//! �p�x�����X�V����
void GameObjectBase::LimitAngle()
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

/*! �p�x�ύX�X�V���� */
#if 1

//! �p�x�S�̕ύX�X�V����
bool GameObjectBase::UpdateAngleChange(float elapsedTime)
{
	//! �p�x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!angleChange.all)
		return false;


	//! �o�ߎ��Ԃ��v��
	angleChangeElapsedTime.all += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = angleChangeElapsedTime.all / angleChangeTime.all;

#if 1
	switch (angleChangeEasing.all)
	{
		//! ���j�A(�⊮����)
	case AngleChangeEasing::Type::Linear:
	{
		//! �p�x�ύX
		angle.x = Easing::Linear(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::Linear(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::Linear(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseIn(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseIn(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseIn(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseOut(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseOut(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseInOut(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseInOut(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseInOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseBack(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseBack(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseBack(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseBounce(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseBounce(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseBounce(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseElastic(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseElastic(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseElastic(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseInCircle(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseInCircle(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseInCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseOutCircle(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseOutCircle(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseInOutCircle(startAngleChange.x, endAngleChange.x, t);
		angle.y = Easing::EaseInOutCircle(startAngleChange.y, endAngleChange.y, t);
		angle.z = Easing::EaseInOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		angle = endAngleChange;
		angleChange.all = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------//

//! �p�xX�ύX�X�V����
bool GameObjectBase::UpdateAngleXChange(float elapsedTime)
{
	//! �p�x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!angleChange.x)
		return false;


	//! �o�ߎ��Ԃ��v��
	angleChangeElapsedTime.x += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = angleChangeElapsedTime.x / angleChangeTime.x;

#if 1
	switch (angleChangeEasing.x)
	{
		//! ���j�A(�⊮����)
	case AngleChangeEasing::Type::Linear:
	{
		//! �p�x�ύX
		angle.x = Easing::Linear(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseIn(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseOut(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseInOut(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseBack(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseBounce(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseElastic(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseInCircle(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseOutCircle(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! �p�x�ύX
		angle.x = Easing::EaseInOutCircle(startAngleChange.x, endAngleChange.x, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		angle.x = endAngleChange.x;
		angleChange.x = false;
	}

	return true;
}

//! �p�xY�ύX�X�V����
bool GameObjectBase::UpdateAngleYChange(float elapsedTime)
{
	//! �p�x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!angleChange.y)
		return false;


	//! �o�ߎ��Ԃ��v��
	angleChangeElapsedTime.y += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = angleChangeElapsedTime.y / angleChangeTime.y;

#if 1
	switch (angleChangeEasing.y)
	{
		//! ���j�A(�⊮����)
	case AngleChangeEasing::Type::Linear:
	{
		//! �p�x�ύX
		angle.y = Easing::Linear(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseIn(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseOut(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseInOut(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseBack(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseBounce(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseElastic(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseInCircle(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseOutCircle(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! �p�x�ύX
		angle.y = Easing::EaseInOutCircle(startAngleChange.y, endAngleChange.y, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		angle.y = endAngleChange.y;
		angleChange.y = false;
	}

	return true;
}

//! �p�xZ�ύX�X�V����
bool GameObjectBase::UpdateAngleZChange(float elapsedTime)
{
	//! �p�x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!angleChange.z)
		return false;


	//! �o�ߎ��Ԃ��v��
	angleChangeElapsedTime.z += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = angleChangeElapsedTime.z / angleChangeTime.z;

#if 1
	switch (angleChangeEasing.z)
	{
		//! ���j�A(�⊮����)
	case AngleChangeEasing::Type::Linear:
	{
		//! �p�x�ύX
		angle.z = Easing::Linear(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case AngleChangeEasing::Type::EaseIn:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseIn(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case AngleChangeEasing::Type::EaseOut:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOut:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseInOut(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBack:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseBack(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseBounce:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseBounce(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseElastic:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseElastic(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInCircle:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseInCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseOutCircle:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	case AngleChangeEasing::Type::EaseInOutCircle:
	{
		//! �p�x�ύX
		angle.z = Easing::EaseInOutCircle(startAngleChange.z, endAngleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		angle.z = endAngleChange.z;
		angleChange.z = false;
	}

	return true;
}

//! �p�x�̉�]�̍X�V����(����̒l�͖ڎw�����A�w�肳�ꂽ���Ԓ��ɉ�]����)
bool GameObjectBase::UpdateAngleRotation(float elapsedTime)
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

#endif // �p�x�ύX�X�V����

/********************************************************************************************/

/*! �X�P�[���ύX�X�V���� */
#if 1
//! �X�P�[���S�̕ύX�X�V����
bool GameObjectBase::UpdateScaleChange(float elapsedTime)
{
	//! �傫����ύX���Ȃ��Ȃ�A�������~�߂�
	if (!scaleChange.all)
		return false;


	//! �o�ߎ��Ԃ��v��
	scaleChangeElapsedTime.all += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = scaleChangeElapsedTime.all / scaleChangeTime.all;

#if 1
	switch (scaleChangeEasing.all)
	{
		//! ���j�A(�⊮����)
	case ScaleChangeEasing::Type::Linear:
	{
		//! �傫���ύX
		scale.x = Easing::Linear(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::Linear(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::Linear(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! �傫���ύX
		scale.x = Easing::EaseIn(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseIn(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseIn(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! �傫���ύX
		scale.x = Easing::EaseOut(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseOut(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! �傫���ύX
		scale.x = Easing::EaseInOut(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseInOut(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseInOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! �傫���ύX
		scale.x = Easing::EaseBack(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseBack(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseBack(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! �傫���ύX
		scale.x = Easing::EaseBounce(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseBounce(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseBounce(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! �傫���ύX
		scale.x = Easing::EaseElastic(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseElastic(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseElastic(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! �傫���ύX
		scale.x = Easing::EaseInCircle(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseInCircle(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseInCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! �傫���ύX
		scale.x = Easing::EaseOutCircle(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseOutCircle(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! �傫���ύX
		scale.x = Easing::EaseInOutCircle(startScaleChange.x, endScaleChange.x, t);
		scale.y = Easing::EaseInOutCircle(startScaleChange.y, endScaleChange.y, t);
		scale.z = Easing::EaseInOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		scale = endScaleChange;
		scaleChange.all = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------//

//! �X�P�[��X�ύX�X�V����
bool GameObjectBase::UpdateScaleXChange(float elapsedTime)
{
	//! �X�P�[����ς��Ȃ��Ȃ�A�������~�߂�
	if (!scaleChange.x)
		return false;


	//! �o�ߎ��Ԃ��v��
	scaleChangeElapsedTime.x += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = scaleChangeElapsedTime.x / scaleChangeTime.x;

#if 1
	switch (scaleChangeEasing.x)
	{
		//! ���j�A(�⊮����)
	case ScaleChangeEasing::Type::Linear:
	{
		//! �X�P�[���ύX
		scale.x = Easing::Linear(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseIn(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseOut(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseInOut(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseBack(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseBounce(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseElastic(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseInCircle(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseOutCircle(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! �X�P�[���ύX
		scale.x = Easing::EaseInOutCircle(startScaleChange.x, endScaleChange.x, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		scale.x = endScaleChange.x;
		scaleChange.x = false;
	}

	return true;
}

//! �X�P�[��Y�ύX�X�V����
bool GameObjectBase::UpdateScaleYChange(float elapsedTime)
{
	//! �X�P�[����ς��Ȃ��Ȃ�A�������~�߂�
	if (!scaleChange.y)
		return false;


	//! �o�ߎ��Ԃ��v��
	scaleChangeElapsedTime.y += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = scaleChangeElapsedTime.y / scaleChangeTime.y;

#if 1
	switch (scaleChangeEasing.y)
	{
		//! ���j�A(�⊮����)
	case ScaleChangeEasing::Type::Linear:
	{
		//! �X�P�[���ύX
		scale.y = Easing::Linear(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseIn(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseOut(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseInOut(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseBack(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseBounce(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseElastic(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseInCircle(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseOutCircle(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! �X�P�[���ύX
		scale.y = Easing::EaseInOutCircle(startScaleChange.y, endScaleChange.y, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		scale.y = endScaleChange.y;
		scaleChange.y = false;
	}

	return true;
}

//! �X�P�[��Z�ύX�X�V����
bool GameObjectBase::UpdateScaleZChange(float elapsedTime)
{
	//! �X�P�[����ς��Ȃ��Ȃ�A�������~�߂�
	if (!scaleChange.z)
		return false;


	//! �o�ߎ��Ԃ��v��
	scaleChangeElapsedTime.z += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = scaleChangeElapsedTime.z / scaleChangeTime.z;

#if 1
	switch (scaleChangeEasing.z)
	{
		//! ���j�A(�⊮����)
	case ScaleChangeEasing::Type::Linear:
	{
		//! �X�P�[���ύX
		scale.z = Easing::Linear(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! �C�[�Y�C��(����)
	case ScaleChangeEasing::Type::EaseIn:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseIn(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	//! �C�[�Y�A�E�g(����)
	case ScaleChangeEasing::Type::EaseOut:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOut:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseInOut(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBack:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseBack(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseBounce:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseBounce(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseElastic:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseElastic(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInCircle:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseInCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseOutCircle:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	case ScaleChangeEasing::Type::EaseInOutCircle:
	{
		//! �X�P�[���ύX
		scale.z = Easing::EaseInOutCircle(startScaleChange.z, endScaleChange.z, t);
	}
	break;
	default:
		break;
	}
#endif

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		scale.z = endScaleChange.z;
		scaleChange.z = false;
	}

	return true;
}

#endif // �X�P�[���ύX�X�V����

//------------------------------------------------------------------------------------------//

/*! �����蔻��*/
#if 1

//! �����蔻��̈ʒu��ݒ�
void GameObjectBase::CollisionPosSettings()
{
	collisionPos = {
		position.x + collisionOffset.x,
		position.y + collisionOffset.y,
		position.z + collisionOffset.z
	};
}

//! �����蔻��̃I�t�Z�b�g�ύX�X�V����
bool GameObjectBase::UpdateCollisionOffsetChange(float elapsedTime)
{
	//! �����蔻��̃I�t�Z�b�g��ς��Ȃ��Ȃ�A�������~�߂�
	if (!collisionOffsetChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	collisionOffsetChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = collisionOffsetChangeElapsedTime / collisionOffsetChangeTime;

	//! �����蔻��̃I�t�Z�b�g��ύX
	collisionOffset.x = Easing::Linear(collisionOffset.x, endCollisionOffsetChange.x, t);
	collisionOffset.y = Easing::Linear(collisionOffset.y, endCollisionOffsetChange.y, t);
	collisionOffset.z = Easing::Linear(collisionOffset.z, endCollisionOffsetChange.z, t);

	//! �����蔻��̃I�t�Z�b�g�ύX���I�������A�������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		collisionOffset = endCollisionOffsetChange;
		collisionOffsetChange = false;
	}

	return true;
}

//! ���a�ύX�X�V����
bool GameObjectBase::UpdateRadiusChange(float elapsedTime)
{
	//! ���a��ύX���Ȃ��Ȃ�A�������~�߂�
	if (!radiusChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	radiusChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = radiusChangeElapsedTime / radiusChangeTime;

	//! ���a��ύX
	radius = Easing::Linear(startRadiusChange, endRadiusChange, t);

	//! ���a�̕ύX���I�������
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		radius = endRadiusChange;
		radiusChange = false;
	}

	return true;
}

//! �����ύX�X�V����
bool GameObjectBase::UpdateHeightChange(float elapsedTime)
{
	//! ������ύX���Ȃ��Ȃ�A�������~�߂�
	if (!heightChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	heightChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = heightChangeElapsedTime / heightChangeTime;

	//! ������ύX
	height = Easing::Linear(startHeightChange, endHeightChange, t);

	//! �����̕ύX���I�������
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		height = endHeightChange;
		heightChange = false;
	}

	return true;
}

//! ���ύX�X�V����
bool GameObjectBase::UpdateWidthChange(float elapsedTime)
{
	//! ����ύX���Ȃ��Ȃ�A�������~�߂�
	if (!widthChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	widthChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = widthChangeElapsedTime / widthChangeTime;

	//! ����ύX
	width = Easing::Linear(startWidthChange, endWidthChange, t);

	//! ���̕ύX���I�������
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		width = endWidthChange;
		widthChange = false;
	}

	return true;
}

#endif // �����蔻��

/********************************************************************************************/

/*! �`��֘A */
#if 1

//! �}�e���A���̐F�ύX�X�V����
bool GameObjectBase::UpdateMaterialColorChange(float elapsedTime)
{
	//! �}�e���A���̐F��ς��Ȃ��Ȃ�A�������~�߂�
	if (!materialColorChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	materialColorChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = materialColorChangeElapsedTime / materialColorChangeTime;

	//! �}�e���A���̐F��ύX
	materialColor.x = Easing::Linear(materialColor.x, endMaterialColorChange.x, t);
	materialColor.y = Easing::Linear(materialColor.y, endMaterialColorChange.y, t);
	materialColor.z = Easing::Linear(materialColor.z, endMaterialColorChange.z, t);

	//! �}�e���A���̐F�ύX���I�������A�������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		materialColor = endMaterialColorChange;
		materialColorChange = false;
	}

	return true;
}

//! �s�����x�ύX�X�V����
bool GameObjectBase::UpdateOpacityChange(float elapsedTime)
{
	//! �s�����x��ύX���Ȃ��Ȃ�A�������~�߂�
	if (!opacityChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	opacityChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = opacityChangeElapsedTime / opacityChangeTime;

	//! �s�����x��ύX
	opacity = Easing::Linear(startOpacityChange, endOpacityChange, t);

	//! �s�����x�̕ύX���I�������
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		opacity = endOpacityChange;
		opacityChange = false;
	}

	return true;
}

//! �G�~�b�V�u�̐F�ύX�X�V����
bool GameObjectBase::UpdateEmissiveColorChange(float elapsedTime)
{
	//! �G�~�b�V�u�̐F��ς��Ȃ��Ȃ�A�������~�߂�
	if (!emissiveColorChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	emissiveColorChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = emissiveColorChangeElapsedTime / emissiveColorChangeTime;

	//!�G�~�b�V�u�̐F��ύX
	emissiveColor.x = Easing::Linear(startEmissiveColorChange.x, endEmissiveColorChange.x, t);
	emissiveColor.y = Easing::Linear(startEmissiveColorChange.y, endEmissiveColorChange.y, t);
	emissiveColor.z = Easing::Linear(startEmissiveColorChange.z, endEmissiveColorChange.z, t);

	//! �������~�߂�
	if (t >= 1.0f);
	{
		//! �l�̂���𖳂���
		emissiveColor = endEmissiveColorChange;
		emissiveColorChange = false;
	}

	return true;
}

//! �G�~�b�V�u�̋����ύX�X�V����
bool GameObjectBase::UpdateEmissiveStrengthChange(float elapsedTime)
{
	//! �G�~�b�V�u�̋�����ς��Ȃ��Ȃ�A�������~�߂�
	if (!emissiveStrengthChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	emissiveStrengthChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = emissiveStrengthChangeElapsedTime / emissiveStrengthChangeTime;

	//! �G�~�b�V�u�̋�����ύX
	emissiveStrength = Easing::Linear(startEmissiveStrengthChange, endEmissiveStrengthChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		emissiveStrength = endEmissiveStrengthChange;
		emissiveStrengthChange = false;
	}

	return true;
}

//! �G�~�b�V�u�̌`�ԕω����o�X�V����
bool GameObjectBase::UpdateEmissivePhaseChange(float elapsedTime)
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

// �G�~�b�V�u�̐F�ύX�X�V����
void GameObjectBase::UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeTime, bool& emissiveColorDown, float elapsedTime)
{
	//! �J���[
	emissiveColor += (emissiveColorDown ? -1 : 1) * emissiveColorChangeTime * elapsedTime;

	//! �F���ŏ��l��菬�����A�܂��́A�ő�l���傫���Ȃ�
	if (emissiveColor < hpImageColorMin || emissiveColor > hpImageColorMax)
	{
		// ���l����
		emissiveColor = std::clamp(emissiveColor,
			emissiveColorMin, emissiveColorMax);

		emissiveColorDown = !emissiveColorDown; // �t���O�𔽓]����
	}
}

#endif // �`��֘A

//******************************************************************************************//

//! ���[�r�[�V�[���̑҂����ԏ���
bool GameObjectBase::UpdateMovieTimer(float elapsedTime)
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

//! ���G���ԍX�V
void GameObjectBase::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
		invincibleTimer -= elapsedTime;
}

//! �h��̓A�b�v���ԍX�V
void GameObjectBase::UpdateDefensesUpTimer(float elapsedTime)
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