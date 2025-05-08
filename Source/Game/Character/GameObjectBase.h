#pragma once

#include<DirectXMath.h>
#include <memory>
#include <stdexcept>
#include <algorithm>

#include "Graphics/Fade.h"
#include "Game/Effect/Effect.h"
#include "Graphics/Model.h"

class GameObjectBase
{
public:
	GameObjectBase() {}
	virtual ~GameObjectBase() {}

/**************************************************************************************/
	/*! �ǂ̃C�[�W���O�ɂ��邩 */
#if 1

	//! �ʒu
	struct PositionChangeEasing
	{
		//! �C�[�W���O�^�C�v
		enum class Type
		{
			Linear,          // ���j�A(�⊮����)
			EaseIn,          // �C�[�Y�C��(����)
			EaseOut,         // �C�[�Y�A�E�g(����)
			EaseInOut,       // �C�[�Y�C���A�E�g(����������)
			EaseBack,        // �o�b�N(�n�܂��I���ɖ߂��������)
			EaseBounce,      // �o�E���h(�e�ޓ���)
			EaseElastic,     // �G���X�e�B�b�N(�e���̂悤�ȓ���)
			EaseInCircle,    // �C�[�Y�C���T�[�N��(�~�`����)
			EaseOutCircle,   // �C�[�Y�A�E�g�T�[�N��(�~�`����)
			EaseInOutCircle, // �C�[�Y�C���A�E�g�T�[�N��(�~�`�������~�`����)
		};

		//! �C�[�W���O�ݒ�
		Type all = Type::Linear;
		Type x = Type::Linear;
		Type y = Type::Linear;
		Type z = Type::Linear;
	}positionChangeEasing;

	//! �p�x
	struct AngleChangeEasing
	{
		enum class Type
		{
			Linear,          // ���j�A(�⊮����)
			EaseIn,          // �C�[�Y�C��(����)
			EaseOut,         // �C�[�Y�A�E�g(����)
			EaseInOut,       // �C�[�Y�C���A�E�g(����������)
			EaseBack,        // �o�b�N(�n�܂��I���ɖ߂��������)
			EaseBounce,      // �o�E���h(�e�ޓ���)
			EaseElastic,     // �G���X�e�B�b�N(�e���̂悤�ȓ���)
			EaseInCircle,    // �C�[�Y�C���T�[�N��(�~�`����)
			EaseOutCircle,   // �C�[�Y�A�E�g�T�[�N��(�~�`����)
			EaseInOutCircle, // �C�[�Y�C���A�E�g�T�[�N��(�~�`�������~�`����)
		};

		//! �C�[�W���O�ݒ�
		Type all = Type::Linear;
		Type x = Type::Linear;
		Type y = Type::Linear;
		Type z = Type::Linear;
	}angleChangeEasing;

	//! �X�P�[��
	struct ScaleChangeEasing
	{
		enum class Type
		{
			Linear,          // ���j�A(�⊮����)
			EaseIn,          // �C�[�Y�C��(����)
			EaseOut,         // �C�[�Y�A�E�g(����)
			EaseInOut,       // �C�[�Y�C���A�E�g(����������)
			EaseBack,        // �o�b�N(�n�܂��I���ɖ߂��������)
			EaseBounce,      // �o�E���h(�e�ޓ���)
			EaseElastic,     // �G���X�e�B�b�N(�e���̂悤�ȓ���)
			EaseInCircle,    // �C�[�Y�C���T�[�N��(�~�`����)
			EaseOutCircle,   // �C�[�Y�A�E�g�T�[�N��(�~�`����)
			EaseInOutCircle, // �C�[�Y�C���A�E�g�T�[�N��(�~�`�������~�`����)
		};

		//! �C�[�W���O�ݒ�
		Type all = Type::Linear;
		Type x = Type::Linear;
		Type y = Type::Linear;
		Type z = Type::Linear;

	}scaleChangeEasing;

#endif

	/*! �ǂ̃C�[�W���O�ɂ��邩(�J�����p) */
#if 1
	//! �^�[�Q�b�g
	enum class CameraTargetChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)
	};

	static CameraTargetChangeEasing cameraTargetChangeEasing;

	//! �p�x
	enum class CameraAngleChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)
	};

	static CameraAngleChangeEasing cameraAngleChangeEasing;

	//! �͈�
	enum class CameraRangeChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)
	};

	static CameraRangeChangeEasing cameraRangeChangeEasing;
#endif

/**************************************************************************************/

	//! �ǂ̓����蔻��ɂ��邩
	enum class CollisionType
	{
		Box,      // �{�b�N�X
		Sphere,   // ��
		Cylinder, // �~��
		Count,    // �v�f�����擾���邽�߂̃_�~�[�G���g��
	}collisionType = CollisionType::Cylinder;

/**************************************************************************************/
	/*! �Z�b�^�[ */
#if 1

	/*! ���̑� */
#if 1

	// �d�͐ݒ�
	void SetGravity(float gravity) { this->gravity = gravity; }

	// �d�͂̃��Z�b�g�̐ݒ�
	void SetGravityReset() { this->gravity = gravityReset; }

	// ���n��Ԃ�����(�����x��ύX�����Ɉʒu�ύX�����ňړ������p)
	void SetIsGroundUnlock() { this->isGround = false; }

	// �X�v���C�g��\���ݒ�
	void SetHideSprites(bool hideSprites) { this->hideSprites = hideSprites; }

#endif

	/*! HP�֘A */
#if 1

	// HP�ݒ�
	void SetHp(int hp)
	{
		this->hp = hp;
		this->hpDamage = hp;
	}

	// HP�𑝂₷�ݒ�
	void SetHpPlus(int hpPlus) { this->hp += hpPlus; }

	// HP�̌`�ԕω��̐ݒ�
	void SetHpPhaseChange(float hpPhaseChangeTimer)
	{
		if (!hpPhaseChange)
		{
			hpPhaseChange = true;               // HP�̌`�ԕω��̉��o������
			this->hpPhaseChangeTimer = hpPhaseChangeTimer; // HP�̌`�ԕω��̉��o�����鎞��
		}
	}

#endif

//---------------------------------------------------------------------------//

	/*! �`��֘A */
#if 1

	// �}�e���A���̐F�̐ݒ�
	void SetMaterialColor(const DirectX::XMFLOAT3 materialColor) { this->materialColor = materialColor; }

	// �}�e���A���̐F�ύX�̐ݒ�
	void SetMaterialColorChange(DirectX::XMFLOAT3 endMaterialColorChange, float materialColorChangeTime)
	{
		if (!this->materialColorChange &&
			(materialColor.x != endMaterialColorChange.x
				|| materialColor.y != endMaterialColorChange.y
				|| materialColor.z != endMaterialColorChange.z))
		{
			this->materialColorChange = true;                    // �}�e���A���̐F��ς���
			this->startMaterialColorChange = this->materialColor;     // �}�e���A���̐F�̕ύX�̊J�n�̒l
			this->endMaterialColorChange = endMaterialColorChange;  // �����܂Ń}�e���A���̐F��ς���
			this->materialColorChangeTime = materialColorChangeTime; // �}�e���A���̐F��ς��鎞��
			this->materialColorChangeElapsedTime = 0.0f;                    // �҂����Ԃ����Z�b�g

			/*! �}�e���A���̐F�͈̔͂𒴂��Ȃ��悤�ɂ��� */
			//! �}�e���A���J���[X
			this->endMaterialColorChange.x = std::clamp(this->endMaterialColorChange.x, 0.0f, 1.0f);
			//! �}�e���A���J���[Y
			this->endMaterialColorChange.y = std::clamp(this->endMaterialColorChange.y, 0.0f, 1.0f);
			//! �}�e���A���J���[Z
			this->endMaterialColorChange.z = std::clamp(this->endMaterialColorChange.z, 0.0f, 1.0f);

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->materialColorChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �s�����x��ݒ�
	void SetOpacity(float radius) { this->radius = radius; }

	// �s�����x�ύX�����̐ݒ�
	void SetOpacityChange(float endOpacityChange, float opacityChangeTime)
	{
		if (!this->opacityChange && opacity != endOpacityChange)
		{
			this->opacityChange = true;              // �s�����x��ς���
			this->startOpacityChange = this->opacity;     // �s�����x�̕ύX�̊J�n�̒l
			this->endOpacityChange = endOpacityChange;  // �����܂ŕs�����x��ς���
			this->opacityChangeTime = opacityChangeTime; // �s�����x��ς��鎞��
			this->opacityChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g

			// �s�����x�͈̔͂𒴂��Ȃ��悤�ɂ���
			this->endOpacityChange = std::clamp(this->endOpacityChange, 0.0f, 1.0f);

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->opacityChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �s�����x�ύX�̉���
	void SetOpacityChangeUnlock() { this->opacityChange = false; }

//-----------------------------------------------------------------------------//

	// �G�~�b�V�u�̐F�̐ݒ�
	void SetEmissiveColor(const DirectX::XMFLOAT3 emissiveColor) { this->emissiveColor = emissiveColor; }

	// �G�~�b�V�u�̐F�ύX�̐ݒ�
	void SetEmissiveColorChange(DirectX::XMFLOAT3 endEmissiveColorChange, float emissiveColorChangeTime)
	{
		if (!this->emissiveColorChange &&
			(this->emissiveColor.x != endEmissiveColorChange.x
				|| this->emissiveColor.y != endEmissiveColorChange.y
				|| this->emissiveColor.z != endEmissiveColorChange.z))
		{
			this->emissiveColorChange = true;                    //�G�~�b�V�u�̐F��ς���
			this->startEmissiveColorChange = this->emissiveColor;     // �G�~�b�V�u�̋����̕ύX�̊J�n�̒l
			this->endEmissiveColorChange = endEmissiveColorChange;  // �����܂ŃG�~�V�b�u�̐F��ς���
			this->emissiveColorChangeTime = emissiveColorChangeTime; //�G�~�b�V�u�̐F��ς��鎞��
			this->emissiveColorChangeElapsedTime = 0.0f;                    // �o�ߎ��Ԃ����Z�b�g

			/*! �G�~�b�V�u�̐F�͈̔͂𒴂��Ȃ��悤�ɂ��� */
			//! �G�~�b�V�u�J���[X
			this->endEmissiveColorChange.x = std::clamp(this->endEmissiveColorChange.x, 0.0f, 1.0f);
			//! �G�~�b�V�u�J���[Y
			this->endEmissiveColorChange.y = std::clamp(this->endEmissiveColorChange.y, 0.0f, 1.0f);
			//! �G�~�b�V�u�J���[Z
			this->endEmissiveColorChange.z = std::clamp(this->endEmissiveColorChange.z, 0.0f, 1.0f);

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->emissiveColorChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �G�~�b�V�u�̋����̐ݒ�
	void SetEmissiveStrength(float emissiveStrength) { this->emissiveStrength = emissiveStrength; }

	// �G�~�b�V�u�̋����ύX�̐ݒ�(�ݒ�̎��ɐF��ς����)
	void SetEmissiveStrengthChange(DirectX::XMFLOAT3 emissiveColor, float endEmissiveStrengthChange, float emissiveStrengthChangeTime)
	{
		if (!this->emissiveStrengthChange && this->emissiveStrength != endEmissiveStrengthChange)
		{
			this->emissiveColor = emissiveColor;              //�G�~�b�V�u�̐F
			this->emissiveStrengthChange = true;                       //�G�~�b�V�u�̋�����ς���
			this->startEmissiveStrengthChange = this->emissiveStrength;     // �G�~�b�V�u�̋����̕ύX�̊J�n�̒l
			this->endEmissiveStrengthChange = endEmissiveStrengthChange;  // �����܂ŃG�~�b�V�u�̋�����ς���
			this->emissiveStrengthChangeTime = emissiveStrengthChangeTime; // �G�~�b�V�u�̋�����ς��鎞��
			this->emissiveStrengthChangeElapsedTime = 0.0f;                       // �o�ߎ��Ԃ����Z�b�g

			// �G�~�b�V�u�̋����͈̔͂𒴂��Ȃ��悤�ɂ���
			this->endEmissiveStrengthChange = std::clamp(this->endEmissiveStrengthChange, 0.0f, 1.0f);

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->emissiveStrengthChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �G�~�V�b�u�̋�����0�ɂ���
	void SetEmissiveStrengthResetChange(float emissiveStrengthChangeTime = 0.1f)
	{
		//! �ύX����������
		this->emissiveStrengthChange = false;

		if (!this->emissiveStrengthChange)
		{
			this->emissiveStrengthChange = true;                       //�G�~�b�V�u�̋�����ς���
			this->startEmissiveStrengthChange = this->emissiveStrength;     // �G�~�b�V�u�̋����̕ύX�̊J�n�̒l
			this->endEmissiveStrengthChange = 0.0f;                       // �����܂ŃG�~�b�V�u�̋�����ς���
			this->emissiveStrengthChangeTime = emissiveStrengthChangeTime; // �G�~�b�V�u�̋�����ς��鎞��
			this->emissiveStrengthChangeElapsedTime = 0.0f;                 // �o�ߎ��Ԃ����Z�b�g

			// �G�~�b�V�u�̋����͈̔͂𒴂��Ȃ��悤�ɂ���
			this->endEmissiveStrengthChange = std::clamp(this->endEmissiveStrengthChange, 0.0f, 1.0f);

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->emissiveStrengthChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �G�~�b�V�u�̋����ύX�̉���
	void SetEmissiveStrengthChangeUnlock() { emissiveStrengthChange = false; }

	// �G�~�b�V�u�̌`�ԕω����o�̐ݒ�
	void SetEmissivePhaseChange(float emissiveTimerPhaseChangeTimer, float emissiveStrength)
	{
		if (!emissivePhaseChange)
		{
			this->emissivePhaseChange = true;                          //�G�~�b�V�u�̌`�ԕω����o������
			this->emissivePhaseChangeTimer = emissiveTimerPhaseChangeTimer; //�G�~�b�V�u�̌`�ԕω����o�����鎞��
			this->emissiveStrength = emissiveStrength;              //�G�~�b�V�u�̋���
		}
	}

#endif

//---------------------------------------------------------------------------//

	//! �����̐ݒ�
	void SetDirection(const DirectX::XMFLOAT3& direction);

	//! �ړ����x�ݒ�
	void SetMoveSpeed(float moveSpeed) { this->moveSpeed = moveSpeed; }

	/*! �ʒu */
#if 1
	// �ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// �ʒuX�ݒ�
	void SetPositionX(float positionX) { this->position.x = positionX; }

	// �ʒuY�ݒ�
	void SetPositionY(float positionY) { this->position.y = positionY; }

	// �ʒuZ�ݒ�
	void SetPositionZ(float positionZ) { this->position.z = positionZ; }

//-------------------------------------------------------------------------//

	// �ʒu�ύX�����̉���(�o�O�h�~�p)
	void SetPositionChangeUnlock()
	{
		this->positionChange.all = false;
		this->positionChange.x = false;
		this->positionChange.y = false;
		this->positionChange.z = false;
	}

	// �ʒu�S�̕ύX�̐ݒ�
	void SetPositionAllChange(const DirectX::XMFLOAT3& endPositionChange,
		float positionChangeTime, PositionChangeEasing::Type positionChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.all &&
			(position.x != endPositionChange.x
				|| position.y != endPositionChange.y
				|| position.z != endPositionChange.z))
		{
			this->positionChange.all = true;                 // �ʒu��ς���
			this->startPositionChange = this->position;       // �ʒu�̕ύX�̊J�n�̒l
			this->endPositionChange = endPositionChange;    // �����܂ňʒu��ς���
			this->positionChangeTime.all = positionChangeTime;   // �ʒu��ς��鎞��
			this->positionChangeEasing.all = positionChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->positionChangeElapsedTime.all = 0.0f;                 // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->positionChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �ʒuX�ύX�̐ݒ�
	void SetPositionXChange(float endPositionXChange,
		float positionXChangeTime, PositionChangeEasing::Type positionXChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.x && position.x != endPositionXChange)
		{
			this->positionChange.x = true;                  // �ʒu��ς���
			this->startPositionChange.x = this->position.x;      // �ʒu�̕ύX�̊J�n�̒l
			this->endPositionChange.x = endPositionXChange;    // �����܂ňʒu��ς���
			this->positionChangeTime.x = positionXChangeTime;   // �ʒu��ς��鎞��
			this->positionChangeEasing.x = positionXChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->positionChangeElapsedTime.x = 0.0f;                  // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->positionChangeTime.x <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �ʒuY�ύX�̐ݒ�
	void SetPositionYChange(float endPositionYChange,
		float positionYChangeTime, PositionChangeEasing::Type positionYChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.y && position.y != endPositionYChange)
		{
			this->positionChange.y = true;                  // �ʒu��ς���
			this->startPositionChange.y = this->position.y;      // �ʒu�̕ύX�̊J�n�̒l
			this->endPositionChange.y = endPositionYChange;    // �����܂ňʒu��ς���
			this->positionChangeTime.y = positionYChangeTime;   // �ʒu��ς��鎞��
			this->positionChangeEasing.y = positionYChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->positionChangeElapsedTime.y = 0.0f;                  // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->positionChangeTime.y <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �ʒuZ�ύX�̐ݒ�
	void SetPositionZChange(float endPositionZChange,
		float positionZChangeTime, PositionChangeEasing::Type positionZChangeEasing = PositionChangeEasing::Type::Linear)
	{
		if (!positionChange.z && position.z != endPositionZChange)
		{
			this->positionChange.z = true;                  // �ʒu��ς���
			this->startPositionChange.z = this->position.z;      // �ʒu�̕ύX�̊J�n�̒l
			this->endPositionChange.z = endPositionZChange;    // �����܂ňʒu��ς���
			this->positionChangeTime.z = positionZChangeTime;   // �ʒu��ς��鎞��
			this->positionChangeEasing.z = positionZChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->positionChangeElapsedTime.z = 0.0f;                  // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->positionChangeTime.z <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

#endif

	/*! �����x */
#if 1

	// �����x��ݒ�(�S��)
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	// �����xX��ݒ�
	void SetVelocityX(float velocityX) { this->velocity.x = velocityX; }

	// �����xY��ݒ�
	void SetVelocityY(float velocityY) { this->velocity.y = velocityY; }

	// �����xZ��ݒ�
	void SetVelocityZ(float velocityZ) { this->velocity.z = velocityZ; }

#endif

//---------------------------------------------------------------------------//

	/*! �p�x */
#if 1

	// �p�x�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// �p�xX�ݒ�
	void SetAngleX(float angleX) { this->angle.x = angleX; }

	// �p�xY�ݒ�
	void SetAngleY(float angleY) { this->angle.y = angleY; }

	// �p�xZ�ݒ�
	void SetAngleZ(float angleZ) { this->angle.z = angleZ; }

//-------------------------------------------------------------------------//

	// �p�x�ύX�̉���
	void SetAngleChangeUnlock()
	{
		this->angleChange.all = false;
		this->angleChange.x = false;
		this->angleChange.y = false;
		this->angleChange.z = false;
	}

	// �p�x�S�̕ύX�̐ݒ�
	void SetAngleAllChange(const DirectX::XMFLOAT3& endAngleChange,
		float angleChangeTime, AngleChangeEasing::Type angleChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!this->angleChange.all &&
			(angle.x != endAngleChange.x
				|| angle.y != endAngleChange.y
				|| angle.z != endAngleChange.z))
		{
			this->angleChange.all = true;              // �p�x��ς���
			this->startAngleChange = this->angle;       // �p�x�̕ύX�̊J�n�̒l
			this->endAngleChange = endAngleChange;    // �����܂Ŋp�x��ς���
			this->angleChangeTime.all = angleChangeTime;   // �p�x��ς��鎞��
			this->angleChangeEasing.all = angleChangeEasing; // �ǂ̃C�[�W���O�ɂ��邩
			this->angleChangeElapsedTime.all = 0.0f;              // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->angleChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �p�xX�ύX�̐ݒ�
	void SetAngleXChange(float endAngleXChange,
		float angleXChangeTime, AngleChangeEasing::Type angleXChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!angleChange.x && angle.x != endAngleXChange)
		{
			this->angleChange.x = true;               // �p�xX��ς���
			this->startAngleChange.x = this->angle.x;      // �p�xX�̕ύX�̊J�n�̒l
			this->endAngleChange.x = endAngleXChange;    // �����܂Ŋp�xX��ς���
			this->angleChangeTime.x = angleXChangeTime;   // �p�xX��ς��鎞��
			this->angleChangeEasing.x = angleXChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->angleChangeElapsedTime.x = 0.0f;               // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->angleChangeTime.x <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �p�xY�ύX�̐ݒ�
	void SetAngleYChange(float endAngleYChange,
		float angleYChangeTime, AngleChangeEasing::Type angleYChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!angleChange.y && angle.y != endAngleYChange)
		{
			this->angleChange.y = true;               // �p�xY��ς���
			this->startAngleChange.y = this->angle.y;      // �p�xY�̕ύX�̊J�n�̒l
			this->endAngleChange.y = endAngleYChange;    // �����܂Ŋp�xY��ς���
			this->angleChangeTime.y = angleYChangeTime;   // �p�xY��ς��鎞��
			this->angleChangeEasing.y = angleYChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->angleChangeElapsedTime.y = 0.0f;               // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->angleChangeTime.y <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �p�xZ�ύX�̐ݒ�
	void SetAngleZChange(float endAngleZChange,
		float angleZChangeTime, AngleChangeEasing::Type angleZChangeEasing = AngleChangeEasing::Type::Linear)
	{
		if (!angleChange.z && angle.z != endAngleZChange)
		{
			this->angleChange.z = true;               // �p�xZ��ς���
			this->startAngleChange.z = this->angle.z;      // �p�xZ�̕ύX�̊J�n�̒l
			this->endAngleChange.z = endAngleZChange;    // �����܂Ŋp�xZ��ς���
			this->angleChangeTime.z = angleZChangeTime;   // �p�xZ��ς��鎞��
			this->angleChangeEasing.z = angleZChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->angleChangeElapsedTime.z = 0.0f;               // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->angleChangeTime.z <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �p�x�̉�]�̐ݒ�(��]���x�̌���(�})�͎蓮)
	void SetAngleRotation(const DirectX::XMFLOAT3& angleRotationSpeed, float rotationTime)
	{
		if (!this->angleRotation)
		{
			this->angleRotation = true;               // �p�x����]������
			this->angleRotationSpeed = angleRotationSpeed; // �p�x�̉�]���x
			this->rotationTime = rotationTime;       // ��]����
		}
	}

	// �p�x�̉�]�̉���
	void SetAngleRotationUnlock() { this->angleRotation = false; }

#endif

//---------------------------------------------------------------------------//

	/*! �X�P�[�� */
#if 1

	// �X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// �X�P�[���ύX�̉���
	void SetScaleChangeUnlock()
	{
		this->scaleChange.all = false;
		this->scaleChange.x = false;
		this->scaleChange.y = false;
		this->scaleChange.z = false;
	}

	// �X�P�[���S�̕ύX�̐ݒ�
	void SetScaleAllChange(const DirectX::XMFLOAT3& endScaleChange,
		float scaleChangeTime, ScaleChangeEasing::Type scaleChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!this->scaleChange.all &&
			(scale.x != endScaleChange.x
				|| scale.y != endScaleChange.y
				|| scale.z != endScaleChange.z))
		{
			this->scaleChange.all = true;              // �X�P�[����ς���
			this->startScaleChange = this->scale;       // �X�P�[���̕ύX�̊J�n�̒l
			this->endScaleChange = endScaleChange;    // �����܂ŃX�P�[����ς���
			this->scaleChangeTime.all = scaleChangeTime;   // �X�P�[����ς��鎞��
			this->scaleChangeEasing.all = scaleChangeEasing; // �ǂ̃C�[�W���O�ɂ��邩
			this->scaleChangeElapsedTime.all = 0.0f;              // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->scaleChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �X�P�[��X�ύX�̐ݒ�
	void SetScaleXChange(float endScaleXChange,
		float scaleXChangeTime, ScaleChangeEasing::Type scaleXChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!scaleChange.x && scale.x != endScaleXChange)
		{
			this->scaleChange.x = true;               // �X�P�[��X��ς���
			this->startScaleChange.x = this->scale.x;      // �X�P�[��X�̕ύX�̊J�n�̒l
			this->endScaleChange.x = endScaleXChange;    // �����܂ŃX�P�[��X��ς���
			this->scaleChangeTime.x = scaleXChangeTime;   // �X�P�[��X��ς��鎞��
			this->scaleChangeEasing.x = scaleXChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->scaleChangeElapsedTime.x = 0.0f;               // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->scaleChangeTime.x <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �X�P�[��Y�ύX�̐ݒ�
	void SetScaleYChange(float endScaleYChange,
		float scaleYChangeTime, ScaleChangeEasing::Type scaleYChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!scaleChange.y && scale.y != endScaleYChange)
		{
			this->scaleChange.y = true;               // �X�P�[��Y��ς���
			this->startScaleChange.y = this->scale.y;      // �X�P�[��Y�̕ύX�̊J�n�̒l
			this->endScaleChange.y = endScaleYChange;    // �����܂ŃX�P�[��Y��ς���
			this->scaleChangeTime.y = scaleYChangeTime;   // �X�P�[��Y��ς��鎞��
			this->scaleChangeEasing.y = scaleYChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->scaleChangeElapsedTime.y = 0.0f;               // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->scaleChangeTime.y <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �X�P�[��Z�ύX�̐ݒ�
	void SetScaleZChange(float endScaleZChange,
		float scaleZChangeTime, ScaleChangeEasing::Type scaleZChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		if (!scaleChange.z && scale.z != endScaleZChange)
		{
			this->scaleChange.z = true;               // �X�P�[��Z��ς���
			this->startScaleChange.z = this->scale.z;      // �X�P�[��Z�̕ύX�̊J�n�̒l
			this->endScaleChange.z = endScaleZChange;    // �����܂ŃX�P�[��Z��ς���
			this->scaleChangeTime.z = scaleZChangeTime;   // �X�P�[��Z��ς��鎞��
			this->scaleChangeEasing.z = scaleZChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->scaleChangeElapsedTime.z = 0.0f;               // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->scaleChangeTime.z <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �X�P�[���̃��Z�b�g�̒l��ݒ�
	void SettingsScaleReset(DirectX::XMFLOAT3& scaleReset) { this->scaleReset = scaleReset; }

	// �X�P�[���̃��Z�b�g
	void SetScaleReset() { this->scale = scaleReset; }

	// �X�P�[�����Z�b�g�̐ݒ�
	void SetScaleResetChange(float scaleResetChangeTime = 0.3f,
		ScaleChangeEasing::Type scaleChangeEasing = ScaleChangeEasing::Type::Linear)
	{
		//! �X�P�[���ύX������
		scaleChange.all = false;
		scaleChange.x = false;
		scaleChange.y = false;
		scaleChange.z = false;

		if ((this->scale.x != this->scaleReset.x
			|| this->scale.y != this->scaleReset.y
			|| this->scale.z != this->scaleReset.z))
		{
			this->scaleChange.all = true;                 // �X�P�[����ς���
			this->startScaleChange = this->scale;          // �X�P�[���̕ύX�̊J�n�̒l
			this->endScaleChange = scaleReset;           // �����܂ŃX�P�[����ς���
			this->scaleChangeTime.all = scaleResetChangeTime; // �X�P�[����ς��鎞��
			this->scaleChangeEasing.all = scaleChangeEasing;    // �ǂ̃C�[�W���O�ɂ��邩
			this->scaleChangeElapsedTime.all = 0.0f;                 // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->scaleChangeTime.all <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

#endif

//---------------------------------------------------------------------------//

	/*! �^�C�}�[ */
#if 1

	// ���[�r�[���̃A�j���[�V�����ݒ�
	void SetMovieAnimation(int movieAnimNum, bool movieAnimLoop)
	{
		this->movieAnimation = false;         // �A�j���[�V������ݒ�ł���悤�ɂ���
		this->movieAnimNum   = movieAnimNum;  // �A�j���[�V�����̔ԍ�
		this->movieAnimLoop  = movieAnimLoop; // �A�j���[�V���������[�v�����邩
	}

	// ���[�r�[���Ԑݒ�
	void SetMovieTime(float movieTime)
	{
		if (!movieScene)
		{
			movieScene = true;
			this->movieTime = movieTime;
		}
	}

	// �A�N�V�����^�C�}�[��ݒ�
	void SetActionTimer(float actionTimer) { this->actionTimer = actionTimer; }

	// �A�N�V�����I���܂ł̑҂����Ԃ�ݒ�
	void SetActionFinishWaitTimer(float actionFinishWaitTimer) { this->actionFinishWaitTimer = actionFinishWaitTimer; }

	// �A�N�V�����I���܂ł̑҂����Ԃ̏�����Ԃ�ݒ�
	void SetActionFinishWaitInitialTimer(float actionFinishWaitInitialTimer) { this->actionFinishWaitInitialTimer = actionFinishWaitInitialTimer; }

	// ���G���Ԑݒ�
	void SetInvincibleTimer(float invincibleTimer) { this->invincibleTimer = invincibleTimer; }

#endif

//---------------------------------------------------------------------------//

	/*! �����蔻�� */
#if 1

	// �����蔻��̃^�C�v��ݒ�
	void SetCollisionType(CollisionType collisionType) { this->collisionType = collisionType; }

	/*! �����蔻��̃I�t�Z�b�g */
#if 1
	// �����蔻��̃I�t�Z�b�g�̐ݒ�
	void SetCollisionOffset(const DirectX::XMFLOAT3& collisionOffset) { this->collisionOffset = collisionOffset; }

	// �����蔻��̃I�t�Z�b�gX��ݒ�
	void SetCollisionOffsetX(float collisionOffsetX) { this->collisionOffset.x = collisionOffsetX; }

	// �����蔻��̃I�t�Z�b�gY��ݒ�
	void SetCollisionOffsetY(float collisionOffsetY) { this->collisionOffset.y = collisionOffsetY; }

	// �����蔻��̃I�t�Z�b�gZ��ݒ�
	void SetCollisionOffsetZ(float collisionOffsetZ) { this->collisionOffset.z = collisionOffsetZ; }

#endif

	// �����蔻��̃I�t�Z�b�g�̃��Z�b�g�̒l��ݒ�
	void SettingsCollisionOffsetReset(DirectX::XMFLOAT3& collisionOffsetReset) { this->collisionOffsetReset = collisionOffsetReset; }

	// �����蔻��̃I�t�Z�b�g�̃��Z�b�g
	void SetCollisionOffsetReset() { this->collisionOffset = this->collisionOffsetReset; }

	// �����蔻��̈ʒu�����ύX�̐ݒ�
	void SetCollisionOffsetChange(const DirectX::XMFLOAT3& endCollisionOffsetChange, float collisionOffsetChangeTime)
	{
		if (!this->collisionOffsetChange &&
			(collisionOffset.x != endCollisionOffsetChange.x
				|| collisionOffset.y != endCollisionOffsetChange.y
				|| collisionOffset.z != endCollisionOffsetChange.z))
		{
			this->collisionOffsetChange = true;                      // �X�P�[����ς���
			this->startCollisionOffsetChange = this->collisionOffset;     // �X�P�[���̕ύX�̊J�n�̒l
			this->endCollisionOffsetChange = endCollisionOffsetChange;  // �����܂ŃX�P�[����ς���
			this->collisionOffsetChangeTime = collisionOffsetChangeTime; // �X�P�[����ς��鎞��
			this->collisionOffsetChangeElapsedTime = 0.0f;                      // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->collisionOffsetChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	/*! ���a */
#if 1

	// ���a�ݒ�
	void SetRadius(float radius) { this->radius = radius; }

	// ���a�ύX�̐ݒ�
	void SetRadiusChange(float endRadiusChange, float radiusChangeTime)
	{
		if (!radiusChange && endRadiusChange != this->endRadiusChange)
		{
			this->radiusChange = true;              // ���a��ς���
			this->startRadiusChange = this->radius;      // ���a�̕ύX�̊J�n�̒l
			this->endRadiusChange = endRadiusChange;   // �����܂Ŕ��a��ς���
			this->radiusChangeTime = radiusChangeTime;  // ���a��ς��鎞��
			this->radiusChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->radiusChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// ���a�ύX�̉���
	void SetRadiusChangeUnlock() { this->radiusChange = false; }

	// ���a�̃��Z�b�g�̒l��ݒ�
	void SettingsRadiusReset(float radiusReset) { this->radiusReset = radiusReset; }

	// ���a�̃��Z�b�g
	void SetRadiusReset() { this->radius = radiusReset; }

	// ���a���Z�b�g�̐ݒ�
	void SetRadiusResetChange(float radiusChangeTime = 0.3f)
	{
		this->radiusChange = false; // ���a�ύX������

		if (this->radius != this->radiusReset)
		{
			this->radiusChange = true;              // ���a��ς���
			this->startRadiusChange = this->radius;      // ���a�̕ύX�̊J�n�̒l
			this->endRadiusChange = this->radiusReset; // �����܂Ŕ��a��ς���
			this->radiusChangeTime = radiusChangeTime;  // ���a��ς��鎞��
			this->radiusChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->radiusChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

#endif

	/*! ���� */
#if 1

	// �����ݒ�
	void SetHeight(float height) { this->height = height; }

	// �����ύX�̐ݒ�
	void SetHeightChange(float endHeightChange, float heightChangeTime)
	{
		if (!heightChange && endHeightChange != this->endHeightChange)
		{
			this->heightChange = true;              // ������ς���
			this->startHeightChange = this->height;      // �����̕ύX�̊J�n�̒l
			this->endHeightChange = endHeightChange;   // �����܂ō�����ς���
			this->heightChangeTime = heightChangeTime;  // ������ς��鎞��
			this->heightChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->heightChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// �����ύX�̉���
	void SetHeightChangeUnlock() { this->heightChange = false; }

	// �����̃��Z�b�g�̒l��ݒ�
	void SettingsHeightReset(float heightReset) { this->heightReset = heightReset; }

	// �����̃��Z�b�g
	void SetHeightReset() { this->height = heightReset; }

	// �������Z�b�g�̐ݒ�
	void SetHeightResetChange(float heightChangeTime = 0.3f)
	{
		this->heightChange = false; // �����ύX������

		if (this->height != this->heightReset)
		{
			this->heightChange = true;              // ������ς���
			this->startHeightChange = this->height;      // �����̕ύX�̊J�n�̒l
			this->endHeightChange = this->heightReset; // �����܂ō�����ς���
			this->heightChangeTime = heightChangeTime;  // ������ς��鎞��
			this->heightChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->heightChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

#endif

	/*! �� */
#if 1

	// ���̐ݒ�
	void SetWidth(float width) { this->width = width; }

	// ���ύX�̐ݒ�
	void SetWidthChange(float endHeightChange, float heightChangeTime)
	{
		if (!widthChange && endWidthChange != this->endWidthChange)
		{
			this->widthChange = true;             // ����ς���
			this->startWidthChange = this->width;      // ���̕ύX�̊J�n�̒l
			this->endWidthChange = endWidthChange;   // �����܂ŕ���ς���
			this->widthChangeTime = widthChangeTime;  // ����ς��鎞��
			this->widthChangeElapsedTime = 0.0f;             // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->widthChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

	// ���ύX�̉���
	void SetWidthChangeUnlock() { this->widthChange = false; }

	// ���̃��Z�b�g�̒l��ݒ�
	void SettingsWidthReset(float widthReset) { this->widthReset = widthReset; }

	// ���̃��Z�b�g
	void SetWidthReset() { this->width = widthReset; }

	// �����Z�b�g�̐ݒ�
	void SetWidthResetChange(float widthChangeTime = 0.3f)
	{
		this->widthChange = false; // ���ύX������

		if (this->width != this->widthReset)
		{
			this->widthChange = true;             // ����ς���
			this->startWidthChange = this->width;      // ���̕ύX�̊J�n�̒l
			this->endWidthChange = this->widthReset; // �����܂ŕ���ς���
			this->widthChangeTime = widthChangeTime;  // ����ς��鎞��
			this->widthChangeElapsedTime = 0.0f;             // �o�ߎ��Ԃ����Z�b�g

			// �ύX���Ԃ̐ݒ肪0�ȉ��̎��ɃG���[���o��
			if (this->widthChangeTime <= 0)
			{
				throw std::out_of_range(u8"�C�[�W���O�o���܂���I�I");
			}
		}
	}

#endif

	// ���s�̐ݒ�
	void SetDepth(float depth) { this->depth = depth; }

	// ���s�̃��Z�b�g
	void SetDepthReset() { this->depth = depthReset; }

#endif

//---------------------------------------------------------------------------//

	/*! �G�֘A */
#if 1

	// ���f���̐ݒ�
	void SetModel(const std::string& modelPath) { model = std::make_unique<Model>(modelPath.c_str()); }

	// �C���f�b�N�X���w�肵�ă��f����ݒ�
	void SetModels(int modelNum, const std::string& modelPath) { models[modelNum] = std::make_unique<Model>(modelPath.c_str()); }

	// �^�[�Q�b�g�̈ʒu��ݒ�
	void SetTargetPosition(const DirectX::XMFLOAT3& targetPosition) { this->targetPosition = targetPosition; }

	// �ڐG�_���[�W�����邩�̐ݒ�
	void SetIsContactDamage(bool isContactDamage) { this->isContactDamage = isContactDamage; }

	// �ڐG�_���[�W�̐ݒ�
	void SetContactDamage(int contactDamage) { this->contactDamage = contactDamage; }

	// �ڐG�_���[�W�̃��Z�b�g
	void SetContactDamageReset() { this->contactDamage = contactDamageReset; }

#endif

//---------------------------------------------------------------------------//
	/*! �J���� */
#if 1

	// �J�����̃^�[�Q�b�g�̈ʒu�ύX�̐ݒ�
	void SetCameraTargetChange(DirectX::XMFLOAT3 endCameraTarget, float cameraTargetChangeTime, CameraTargetChangeEasing cameraTargetChangeEasing = CameraTargetChangeEasing::Linear);

	// �J�����̃^�[�Q�b�g�̈ʒu�̃��Z�b�g�̐ݒ�
	void SetCameraTargetResetChange(float cameraTargetResetChangeTime = 0.5f, CameraTargetChangeEasing cameraTargetChangeEasing = CameraTargetChangeEasing::Linear);

	// �J�����̊p�x�ύX�̐ݒ�
	void SetCameraAngleChange(DirectX::XMFLOAT3 endCameraAngle, float cameraAngleChangeTime, CameraAngleChangeEasing cameraAngleChangeEasing = CameraAngleChangeEasing::Linear);

	// �J�����̊p�x�̃��Z�b�g�̐ݒ�
	void SetCameraAngleResetChange(float cameraAngleResetChangeTime = 0.5f, CameraAngleChangeEasing cameraAngleChangeEasing = CameraAngleChangeEasing::Linear);

	// �J�����͈̔͂̕ύX�̐ݒ�
	void SetCameraRangeChange(float endCameraRange, float cameraRangeChangeTime, CameraRangeChangeEasing cameraRangeChangeEasing = CameraRangeChangeEasing::Linear);

	// �J�����͈̔͂̃��Z�b�g�̐��l
	void SetCameraRangeResetChange(float cameraRangeResetChangeTime = 0.5f, CameraRangeChangeEasing cameraRangeChangeEasing = CameraRangeChangeEasing::Linear);

	// �J�����V�F�C�N�ݒ�
	void SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower);
#endif

//---------------------------------------------------------------------------//
	/*! �V�F�[�_�[ */

	//! �|�X�g�G�t�F�N�g
#if 1
	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
	void SetPostEffectStatus(float contrast = 1.0f,
		float saturation = 0.8f,
		const DirectX::XMFLOAT3& colorFilter = { 1.2f, 1.3f, 1.35f },
		float chromaticAberration = 0);

	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����Z�b�g����
	void SetPostEffectStatusResetChange(
		float contrastChangeTime = 1.5f,
		float saturationChangeTime = 1.5f,
		float colorFilterChangeTime = 1.5f,
		float chromaticAberrationChangeTime = 1.5f,
		float blurStrengthChangeTime = 1.0f);

	/*! �R���g���X�g */
#if 1
	// �R���g���X�g�̐ݒ�
	void SetContrast(float contrast);

	// �R���g���X�g�ύX�̐ݒ�
	void SetContrastChange(float endContrastChange, float contrastChangeTime = 1.5f);

	// �R���g���X�g�̃��Z�b�g
	void SetContrastReset();

	// �R���g���X�g���Z�b�g�ύX�̐ݒ�
	void SetContrastResetChange(float contrastChangeTime = 1.5f);

#endif

	/*! �T�`�����[�V���� */
#if 1

	// �T�`�����[�V����(�F�̍ʓx)�̐ݒ�
	void SetSaturation(float saturation);

	// �T�`�����[�V����(�F�̍ʓx)�ύX�̐ݒ�
	void SetSaturationChange(float endSaturationChange, float saturationChangeTime = 1.5f);

	// �T�`�����[�V�����̃��Z�b�g
	void SetSaturationReset();

	// �T�`�����[�V�������Z�b�g�̕ύX�̐ݒ�
	void SetSaturationResetChange(float saturationChange = 1.5f);

#endif

	/*! �J���[�t�B���^�[ */
#if 1

	// �J���[�t�B���^�[�̐ݒ�
	void SetColorFilter(const DirectX::XMFLOAT3& colorFilter);

	// �J���[�t�B���^�[�ύX�̐ݒ�
	void SetColorFilterChange(DirectX::XMFLOAT3& endColorFilterChange, float colorFilterChangeTime = 1.5f);

	// �J���[�t�B���^�[�̃��Z�b�g
	void SetColorFilterReset();

	// �J���[�t�B���^�[���Z�b�g�̕ύX�̐ݒ�
	void SetColorFilterResetChange(float colorFilterChangeTime = 1.5f);

#endif

	/*! �N���}�e�B�b�N�A�x���[�V���� */
#if 1

	// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�̐ݒ�
	void SetChromaticAberration(float chromaticAberration);

	// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�ύX�̐ݒ�
	void SetChromaticAberrationChange(float endChromaticAberrationChange, float chromaticAberrationChangeTime = 1.5f);

	// �N���}�e�B�b�N�A�x���[�V�����̃��Z�b�g
	void SetChromaticAberrationReset();

	// �N���}�e�B�b�N�A�x���[�V�������Z�b�g�̐ݒ�
	void SetChromaticAberrationResetChange(float chromaticAberrationChangeTime = 1.5f);

#endif

	/*! �u���[�̋��x */
#if 1

	// �u���[�̋��x��ݒ�
	void SetBlurStrength(float blurStrength);

	// �u���[�̋��x�ύX�̐ݒ�
	void SetBlurStrengthChange(float endBlurStrengthChange, float blurStrengthChangeTime = 1.0f);

	// �u���[�̋��x�̃��Z�b�g
	void SetBlurStrengthReset();

	// �u���[�̋��x���Z�b�g�̐ݒ�
	void SetBlurStrengthResetChange(float blurStrengthChangeTime = 1.0f);

#endif

	// �J�����V�F�C�N(�V�F�[�_�[)�̐ݒ�
	void SetCameraShakeShader(float cameraShakeTime = 1.0f, float cameraShakeStrength = 0.2f, DirectX::XMFLOAT2 cameraShakeOffset = { 0, -0.05f }, float cameraShakeSpeed = 50.0f);

	// �J�����V�F�C�N(�V�F�[�_�[)�̉���
	void SetCameraShakeShaderUnlock();

#endif

#endif // �Z�b�^�[

/**************************************************************************************/
	/*! �Q�b�^�[ */
#if 1

	/*! HP */
#if 1

	// HP���擾
	int GetHp() const { return hp; }

	// HP�_���[�W�擾
	int GetHpDamage() const { return hpDamage; }

	// �ő�HP���擾
	int GetMaxHp() const { return maxHp; }

	// HP�����I���擾
	bool GetHpDirectorFinished() const { return hpDirectorFinished; }

#endif

//---------------------------------------------------------------------------//

	/*! �^�C�}�[ */
#if 1

	// �X�e�[�g�ύX�܂ł̎��Ԃ��擾
	float GetStateChangeWaitTimer()  const { return stateChangeWaitTimer; }

	// �A�N�V�����^�C�}�[���擾
	float GetActionTimer() const { return actionTimer; }

	// �A�N�V�����I���܂ł̑҂����Ԃ��擾
	float GetActionFinishWaitTimer() const { return actionFinishWaitTimer; }

	// �A�N�V�����I���܂ł̑҂����Ԃ̏�����Ԃ��擾
	float GetActionFinishWaitInitialTimer() const { return actionFinishWaitInitialTimer; }

	// �q�b�g�X�g�b�v���Ԃ̎擾
	float GetHitStopElapsedTime() const { return hitStopElapsedTime; }

	// ���G���Ԏ擾
	float GetInvincibleTimer() const { return invincibleTimer; }

	// �h��̓A�b�v���Ԏ擾
	float GetDefensesUpTimer() const { return defensesUpTimer; }

#endif

//---------------------------------------------------------------------------//

	/*! ���[�r�[ */
#if 1

	// ���[�r�[�����ǂ������擾
	bool GetMovieScene() const { return movieScene; }

	// ���[�r�[���Ԏ擾
	float GetMovieTime() const { return movieTime; }

#endif

//---------------------------------------------------------------------------//

	/*! �`��֘A */
#if 1

	// �s�����x���擾
	float GetOpacity() const { return opacity; }

	// �s�����x��ς��Ă��邩���擾
	bool GetOpacityChange() const { return opacityChange; }

	// �G�~�V�b�u�̐F���擾
	const DirectX::XMFLOAT3& GetEmissiveColor() const { return emissiveColor; }

	// �G�~�b�V�u�̐F��ς��Ă��邩���擾
	bool GetEmissiveColorChange() const { return emissiveColorChange; }

	// �G�~�V�b�u�̋������擾
	float GetEmissiveStrength() const { return emissiveStrength; }

	// �G�~�b�V�u�̋�����ς��Ă��邩���擾
	bool GetEmissiveStrengthChange() const { return emissiveStrengthChange; }

	// �G�~�V�b�u�̌`�ԕω����o�����Ă��邩���擾
	bool GetEmissivePhaseChange() const { return emissivePhaseChange; }

#endif

//---------------------------------------------------------------------------//

	/*! �|�X�g�G�t�F�N�g */
#if 1

	// �R���g���X�g��ύX���Ă��邩�擾
	bool GetContrastChange();

	// �T�`�����[�V����(�F�̍ʓx)��ύX���Ă��邩���擾
	bool GetSaturationChange();

	// �J���[�t�B���^�[��ύX���Ă��邩���擾
	bool GetColorFilterChange();

	// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))��ύX���Ă��邩���擾
	bool GetChromaticAberrationChange();

	// �u���[�̋��x��ύX���Ă��邩�擾
	bool GetBlurStrengthChange();

#endif

//---------------------------------------------------------------------------//

	// �ǂɐG��Ă��邩
	bool IsWall() const { return isWall; }

	// �n�ʂɐڒn���Ă��邩
	bool IsGround() const { return isGround; }

	// �g�����X�t�H�[���擾
	const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	// �����擾
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }

	/*! �ʒu */
#if 1

	// �ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// �ʒu�ύX�����Ă��邩���擾
	bool GetPositionAllChange() const { return positionChange.all; }

	// �ʒuX�ύX�����Ă��邩���擾
	bool GetPositionXChange() const { return positionChange.x; }

	// �ʒuY�ύX�����Ă��邩���擾
	bool GetPositionYChange() const { return positionChange.y; }

	// �ʒuZ�ύX�����Ă��邩���擾
	bool GetPositionZChange() const { return positionChange.z; }

#endif

	// �����x�擾
	const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }

//---------------------------------------------------------------------------//

	/*! �p�x */
#if 1

	// �p�x�擾
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	// �p�x�ύX�����Ă��邩���擾
	bool GetAngleAllChange() const { return angleChange.all; }

	// �p�xX�ύX�����Ă��邩���擾
	bool GetAngleXChange() const { return angleChange.x; }

	// �p�xY�ύX�����Ă��邩���擾
	bool GetAngleYChange() const { return angleChange.y; }

	// �p�xZ�ύX�����Ă��邩���擾
	bool GetAngleZChange() const { return angleChange.z; }

	// ��]���Ă��邩���擾
	bool GetAngleRotation() const { return angleRotation; }

#endif

//---------------------------------------------------------------------------//

	/*! �X�P�[�� */
#if 1

	// �X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	// �X�P�[�����Z�b�g�̎擾
	const DirectX::XMFLOAT3& GetScaleReset() const { return scaleReset; }

	// �X�P�[���ύX�����Ă��邩���擾
	bool GetScaleAllChange() const { return scaleChange.all; }

	// �X�P�[��X�ύX�����Ă��邩���擾
	bool GetScaleXChange() const { return scaleChange.x; }

	// �X�P�[��Y�ύX�����Ă��邩���擾
	bool GetScaleYChange() const { return scaleChange.y; }

	// �X�P�[��Z�ύX�����Ă��邩���擾
	bool GetScaleZChange() const { return scaleChange.z; }

#endif

//---------------------------------------------------------------------------//

	/*! �����蔻�� */
#if 1

	// �����蔻��̃^�C�v�擾
	CollisionType GetCollisionType() const { return collisionType; }

	// �����蔻��̈ʒu�擾
	const DirectX::XMFLOAT3& GetCollisionPos() const { return collisionPos; }

	// �����蔻��̒������擾
	const DirectX::XMFLOAT3& GetCollisionOffset() const { return collisionOffset; }

	// ���a�擾
	float GetRadius() const { return radius; }

	// �����擾
	float GetHeight() const { return height; }

	// ���̒������擾
	float GetWidth() const { return width; }

	// ���s�����擾
	float GetDepth() const { return depth; }

#endif

#endif // �Q�b�^�[

protected:

//!\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
	/*! �X�V�����Ȃ� */

/*! HP�֘A */
#if 1

	// HP�Ǘ�
	void HpControll(float elapsedTime);

	// HP���o
	bool HpDirector(float elapsedTime);

	// HP�s���`�X�V����(�G�̂�)
	bool UpdateHpPinchEnemy();

	/*! ��ԍX�V���� */

	// HP�V�F�C�N
	bool UpdateHpShake(float elapsedTime);

	// HP�̌`�ԕω����o
	bool UpdateHpPhaseChange(float elapsedTime);

	// HP�摜�̐F�ύX�X�V����
	void UpdateHpColorChange(float& hpImageColor, float& hpImageColorChangeSpeed, bool& hpImageColorUp, float elapsedTime);

	// HP�Q�[�W�̐F�ύX�X�V����
	void UpdateHpSpriteChange(float& hpSpriteColor, float& hpSpriteColorChangeSpeed, bool& hpSpriteColorDown, float elapsedTime);

#endif

protected:

	//! �I�u�W�F�N�g
	enum class Object
	{
		Character,  // �L�����N�^�[
		Stage,      // �X�e�[�W
		Projectile, // ��ѓ���
	}object;

protected:

	//! ��ԍX�V����
	void UpdateGameObjectBaseState(float elapsedTime, Object object = Object::Character);

	// ��ԍX�V�����֘A(��̊֐��ɂ܂Ƃ߂��Ă���)
#if 1

	// �q�b�g�X�g�b�v�X�V����
	bool UpdateHitStop(float elapsedTime);

	// �s��X�V����
	void UpdateTransform();

	// �s��X�V����(�X�e�[�W)
	void UpdateTransform_Stage();

	// �s��X�V����(��ѓ���)
	void UpdateTransform_Projectile();

	// ���C�L���X�g�X�V����
	void UpdateProjectileRaycast();

	/*! ���͏��� */
#if 1
	//! �����x�X�V����
	void UpdateVelocity(float elapsedTime);

	// �������͍X�V����
	void UpdateVerticalVelocity(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);

	// �������͍X�V����
	void UpdateHorizontalVelocity(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateHorizontalMove(float elapsedTime);

#endif

	/*! �ʒu*/
#if 1
	// �ʒu�S�̕ύX�X�V����
	bool UpdatePositionAllChange(float elapsedTime);

	// �ʒuX�ύX�X�V����
	bool UpdatePositionXChange(float elapsedTime);

	// �ʒuY�ύX�X�V����
	bool UpdatePositionYChange(float elapsedTime);

	// �ʒuZ�ύX�X�V����
	bool UpdatePositionZChange(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! �p�x */
#if 1

	// �p�x��������
	void LimitAngle();

	// �p�x�S�̕ύX�X�V����
	bool UpdateAngleChange(float elapsedTime);

	// �p�xX�ύX�X�V����
	bool UpdateAngleXChange(float elapsedTime);

	// �p�xY�ύX�X�V����
	bool UpdateAngleYChange(float elapsedTime);

	// �p�xZ�ύX�X�V����
	bool UpdateAngleZChange(float elapsedTime);

	// �p�x�̉�]�̍X�V����(����̒l�͖ڎw�����A�w�肳�ꂽ���Ԓ��ɉ�]����)
	bool UpdateAngleRotation(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! �X�P�[�� */
#if 1

	// �X�P�[���S�̕ύX�X�V����
	bool UpdateScaleChange(float elapsedTime);

	// �X�P�[��X�ύX�X�V����
	bool UpdateScaleXChange(float elapsedTime);

	// �X�P�[��Y�ύX�X�V����
	bool UpdateScaleYChange(float elapsedTime);

	// �X�P�[��Z�ύX�X�V����
	bool UpdateScaleZChange(float elapsedTime);

#endif

	//---------------------------------------------------------------------------//

		/*! �`��֘A */
#if 1
	// �}�e���A���̐F�ύX�X�V����
	bool UpdateMaterialColorChange(float elapsedTime);

	// �s�����x�ύX�X�V����
	bool UpdateOpacityChange(float elapsedTime);

	// �G�~�b�V�u�̐F�ύX�X�V����
	bool UpdateEmissiveColorChange(float elapsedTime);

	//�G�~�b�V�u�̋����ύX�X�V����
	bool UpdateEmissiveStrengthChange(float elapsedTime);

	//�G�~�b�V�u�̌`�ԕω����o�X�V����
	bool UpdateEmissivePhaseChange(float elapsedTime);

	//�G�~�b�V�u�̐F�ύX�X�V����(�`�ԕω�)
	void UpdateEmissiveColorPhaseChange(float& emissiveColor, float& emissiveColorChangeTime, bool& emissiveColorUp, float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! �����蔻�� */
#if 1

	// �����蔻��̈ʒu��ݒ�
	void CollisionPosSettings();

	// �����蔻��̃I�t�Z�b�g�ύX�X�V����
	bool UpdateCollisionOffsetChange(float elapsedTime);

	// ���a�ύX�X�V����
	bool UpdateRadiusChange(float elapsedTime);

	// �����ύX�X�V����
	bool UpdateHeightChange(float elapsedTime);

	// ���ύX�X�V����
	bool UpdateWidthChange(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

	/*! ���̑� */
#if 1

	// ���[�r�[���ԍX�V����
	bool UpdateMovieTimer(float elapsedTime);

	// ���G���ԍX�V
	void UpdateInvincibleTimer(float elapsedTime);

	// �h��̓A�b�v���ԍX�V
	void UpdateDefensesUpTimer(float elapsedTime);

#endif

//---------------------------------------------------------------------------//

#endif

	// ���n�������ɌĂ΂��
	virtual void OnLanding() {}

	// �_���[�W���󂯂����ɌĂ΂��
	virtual void OnDamaged() {};

	// ���S�������ɌĂ΂��
	virtual void OnDead();

protected:
	// ����
	DirectX::XMFLOAT3   direction = { 0, 0, 1 };
	// �g�����X�t�H�[��
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	// �I���h�g�����X�t�H�[��
	DirectX::XMFLOAT4X4 oldTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	float gravity      = 0.0f; // �d��
	float gravityReset = 0.0f; // �d�͂̃��Z�b�g

	bool isWall   = false; // �ǂɐG��Ă���
	bool isGround = false; // �n�ʂɂ���

	float moveSpeed      = 10.0f; // �ړ����x
	float moveSpeedReset = 10.0f; // �ړ����x�̃��Z�b�g

	/*! �����蔻�� */
#if 1

	DirectX::XMFLOAT3 collisionPos = { 0, 0, 0 }; // �����蔻��̈ʒu

	DirectX::XMFLOAT3 collisionOffset = { 0, 0, 0 }; // �����蔻��̃I�t�Z�b�g
	DirectX::XMFLOAT3 collisionOffsetReset = { 0, 0, 0 }; // �����蔻��̃I�t�Z�b�g�̃��Z�b�g
	bool collisionOffsetChange = false;       // �����蔻��̃I�t�Z�b�g��ς��邩

	DirectX::XMFLOAT3 startCollisionOffsetChange = { 0, 0, 0 }; // �����蔻��̃I�t�Z�b�g�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endCollisionOffsetChange = { 0, 0, 0 }; // �����܂œ����蔻��̃I�t�Z�b�g��ς���
	float collisionOffsetChangeTime = 0.0f;        // �����蔻��̃I�t�Z�b�g��ς��鎞��
	float collisionOffsetChangeElapsedTime = 0.0f;        // �����蔻��̃I�t�Z�b�g�ύX�̌o�ߎ���

	/*! ���a */
#if 1
	float radius = 1.0f;  // ���a
	bool  radiusChange = false; // ���a��ς��邩

	float startRadiusChange = 0.0f; // ���a�̕ύX�̊J�n�̒l
	float endRadiusChange = 0.0f; // �����܂Ŕ��a��ς���
	float radiusChangeTime = 0.0f; // ���a��ς��鎞��
	float radiusChangeElapsedTime = 0.0f; // ���a�ύX�̌o�ߎ���
#endif

	/*! ���� */
#if 1
	float height = 1.0f;  // ����
	bool  heightChange = false; // ������ς��邩

	float startHeightChange = 0.0f; // �����̕ύX�̊J�n�̒l
	float endHeightChange = 0.0f; // �����܂ō�����ς���
	float heightChangeTime = 0.0f; // ������ς��鎞��
	float heightChangeElapsedTime = 0.0f; // �����ύX�̌o�ߎ���
#endif

	//! �{�b�N�X
#if 1
	/*! �� */
#if 1
	float width = 1.0f;  // ��
	bool  widthChange = false; // ����ς��邩

	float startWidthChange = 0.0f; // �����̕ύX�̊J�n�̒l
	float endWidthChange = 0.0f; // �����܂ŕ���ς���
	float widthChangeTime = 0.0f; // ����ς��鎞��
	float widthChangeElapsedTime = 0.0f; // ���ύX�̌o�ߎ���
#endif

	float depth = 1.0f;  //! ���s��

#endif

	//! ���Z�b�g
	float radiusReset = 1.0f; // ���a
	float heightReset = 1.0f; // ����
	//! �{�b�N�X
	float widthReset = 1.0f; // ��
	float depthReset = 1.0f; // ���s��

#endif

/*****************************************************************************************************/
	/*! �ʒu */
#if 1

	DirectX::XMFLOAT3 position = { 0, 0, 0 }; // �ʒu
	DirectX::XMFLOAT3 startPositionChange = { 0, 0, 0 }; // �ʒu�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endPositionChange = { 0, 0, 0 }; // �����܂ňʒu��ς���

	DirectX::XMFLOAT3 velocity = { 0, 0, 0 }; // �����x

	//! �ʒu��ύX���邩
	struct PositionChange
	{
		bool all = false; // �S�Ă̈ʒu��ύX���邩
		bool x = false; // �ʒuX��ύX���邩
		bool y = false; // �ʒuY��ύX���邩
		bool z = false; // �ʒuZ��ύX���邩
	}positionChange;

	//! �ʒu��ς��鎞��
	struct PositionChangeTime
	{
		float all = 0.0f; // �S�Ă̈ʒu��ς��鎞��
		float x = 0.0f; // �ʒuX��ς��鎞��
		float y = 0.0f; // �ʒuY��ς��鎞��
		float z = 0.0f; // �ʒuZ��ς��鎞��
	}positionChangeTime;

	//! �ʒu�ύX�̌o�ߎ���
	struct PositionChangeElapsedTime
	{
		float all = 0.0f; // �S�Ă̈ʒu�ύX�̌o�ߎ���
		float x = 0.0f; // �ʒuX�ύX�̌o�ߎ���
		float y = 0.0f; // �ʒuY�ύX�̌o�ߎ���
		float z = 0.0f; // �ʒuZ�ύX�̌o�ߎ���
	}positionChangeElapsedTime;

#endif

/*****************************************************************************************************/
	/*! �p�x */
#if 1

	DirectX::XMFLOAT3 angle = { 0, 0, 0 }; // �p�x
	DirectX::XMFLOAT3 startAngleChange = { 0, 0, 0 }; // �p�x�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endAngleChange = { 0, 0, 0 }; // �����܂Ŋp�x��ς���

	//! �p�x��ύX���邩
	struct AngleChange
	{
		bool all = false; // �S�Ă̊p�x��ύX���邩
		bool x = false; // �p�xX��ύX���邩
		bool y = false; // �p�xY��ύX���邩
		bool z = false; // �p�xZ��ύX���邩
	}angleChange;

	//! �p�x��ς��鎞��
	struct AngleChangeTime
	{
		float all = 0.0f; // �S�Ă̊p�x��ς��鎞��
		float x = 0.0f; // �p�xX��ς��鎞��
		float y = 0.0f; // �p�xY��ς��鎞��
		float z = 0.0f; // �p�xZ��ς��鎞��
	}angleChangeTime;

	//! �p�x�ύX�̌o�ߎ���
	struct AngleChangeElapsedTime
	{
		float all = 0.0f; // �S�Ă̊p�x�ύX�̌o�ߎ���
		float x = 0.0f; // �p�xX�ύX�̌o�ߎ���
		float y = 0.0f; // �p�xY�ύX�̌o�ߎ���
		float z = 0.0f; // �p�xZ�ύX�̌o�ߎ���
	}angleChangeElapsedTime;

	//! ��](����̒l�͖ڎw���Ȃ�)
	float angleRotation = false; // �p�x����]�����邩
	float rotationTime = 0.0f;  // ��]����
	DirectX::XMFLOAT3 angleRotationSpeed = { 0, 0, 0 }; // �p�x�̉�]���x

#endif

/*****************************************************************************************************/
	/*! �X�P�[�� */
#if 1 

	DirectX::XMFLOAT3 scale = { 1, 1, 1 };             // �X�P�[��
	DirectX::XMFLOAT3 scaleReset = { 0.02f, 0.02f, 0.02f }; // ��������̓���ő傫����ύX�����ۂɖ߂��p

	DirectX::XMFLOAT3 startScaleChange = { 0, 0, 0 }; // �X�P�[���̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endScaleChange = { 0, 0, 0 }; // �����܂ŃX�P�[����ς���

	//! �X�P�[����ύX���邩
	struct ScaleChange
	{
		bool all = false; // �S�ẴX�P�[����ύX���邩
		bool x = false; // �X�P�[��X��ύX���邩
		bool y = false; // �X�P�[��Y��ύX���邩
		bool z = false; // �X�P�[��Z��ύX���邩
	}scaleChange;

	//! �X�P�[����ς��鎞��
	struct ScaleChangeTime
	{
		float all = 0.0f; // �S�ẴX�P�[����ς��鎞��
		float x = 0.0f; // �X�P�[��X��ς��鎞��
		float y = 0.0f; // �X�P�[��Y��ς��鎞��
		float z = 0.0f; // �X�P�[��Z��ς��鎞��
	}scaleChangeTime;

	//! �X�P�[���ύX�̌o�ߎ���
	struct ScaleChangeElapsedTime
	{
		float all = 0.0f; // �S�ẴX�P�[���ύX�̌o�ߎ���
		float x = 0.0f; // �X�P�[��X�ύX�̌o�ߎ���
		float y = 0.0f; // �X�P�[��Y�ύX�̌o�ߎ���
		float z = 0.0f; // �X�P�[��Z�ύX�̌o�ߎ���
	}scaleChangeElapsedTime;

#endif

/*****************************************************************************************************/
	/*! �}�e���A�� */
#if 1
	DirectX::XMFLOAT3 materialColor = { 1,1,1 }; // �}�e���A���̐F
	bool materialColorChange = false;     // �}�e���A���̐F��ύX���邩

	DirectX::XMFLOAT3 startMaterialColorChange = { 0, 0, 0 }; // �}�e���A���̐F�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endMaterialColorChange = { 0, 0, 0 }; // �����܂Ń}�e���A���̐F��ς���
	float materialColorChangeTime = 0.0f;        // �}�e���A���̐F��ς��鎞��
	float materialColorChangeElapsedTime = 0.0f;        // �}�e���A���̐F�ύX�̌o�ߎ���

	//! �s�����x
	float opacity = 1.0f;  // �s�����x
	bool  opacityChange = false; // �s�����x��ύX���邩

	float startOpacityChange = 0.0f;  // �s�����x�̕ύX�̊J�n�̒l
	float endOpacityChange = 0.0f;  // �����܂ŕs�����x��ς��� 
	float opacityChangeTime = 0.0f;  // �s�����x��ς��鎞��
	float opacityChangeElapsedTime = 0.0f;  // �s�����x�ύX�̌o�ߎ���
#endif

/*****************************************************************************************************/
	/*! �G�~�b�V�u */
#if 1
	DirectX::XMFLOAT3 emissiveColor = { 0.0f, 0.0f, 0.0f }; //�G�~�b�V�u�J���[
	bool emissiveColorChange = false;                //�G�~�b�V�u�̐F��ς��邩

	DirectX::XMFLOAT3 startEmissiveColorChange = { 0, 0, 0 }; // �G�~�V�b�u�̐F�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endEmissiveColorChange = { 0, 0, 0 }; // �����܂ŃG�~�V�b�u�̐F��ς���
	float emissiveColorChangeTime = 0.0f;        // �G�~�b�V�u�̐F��ς��鎞��
	float emissiveColorChangeElapsedTime = 0.0f;        // �G�~�b�V�u�̐F�ύX�̌o�ߎ���

	//! �G�~�b�V�u�̋���
	float emissiveStrength = 0.0f;  // �G�~�b�V�u�̋���
	bool  emissiveStrengthChange = false; // �G�~�b�V�u�̋�����ς��邩

	float startEmissiveStrengthChange = 0.0f; // �G�~�b�V�u�̋����̕ύX�̊J�n�̒l
	float endEmissiveStrengthChange = 0.0f; // �����܂ŃG�~�V�b�u�̋�����ς���
	float emissiveStrengthChangeTime = 0.0f; // �G�~�V�b�u�̋�����ς��鎞��
	float emissiveStrengthChangeElapsedTime = 0.0f; // �G�~�b�V�u�̋����ύX�̌o�ߎ���
#endif

//-------------------------------------------------------------------------------------------------------//

	/*! �`�ԕω����o(�G�~�V�b�u) */
#if 1
	bool emissivePhaseChange = false; //�G�~�b�V�u�̌`�ԕω����o�����邩
	float emissivePhaseChangeTimer = 0.0f;  //�G�~�b�V�u�̌`�ԕω����o�����鎞��
	float emissiveColorMin = 0.2f;  //�G�~�b�V�u�̐F(�ŏ��l)
	float emissiveColorMax = 1.0f;  //�G�~�b�V�u�̐F(�ő�l)
	DirectX::XMFLOAT3
		emissiveColorPhaseChangeSpeed = { 4.0f, 3.8f, 4.4f }; //�G�~�b�V�u�̐F��ς��鎞��(�`�ԕω����o)
	struct EmissiveColorChangeDown
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}emissiveColorChangeDown;   //�G�~�b�V�u�̐F�����炷��(��̕��Ƃ͎g�p�p�r���Ⴄ)
#endif

/*****************************************************************************************************/
	/*! ���[�r�[�֘A */

	bool  movieScene = false;  // ���[�r�[�����ǂ���
	float movieTime = 0.0f;   // ���[�r�[����
	bool  movieAnimation = false;  // ���[�r�[���̃A�j���[�V������������
	int   movieAnimNum = 0;      // ���[�r�[���̃A�j���[�V�����ԍ�
	bool  movieAnimLoop = false;  // ���[�r�[���A�j���[�V�������Ƀ��[�v�����邩

/*****************************************************************************************************/
	/*! HP�֘A */
#if 1

	int   hp = 10;    // HP
	int   maxHp = 5;    // �ő�HP
	bool  hpDirectorFinished = false; // HP�̉��o���I�������
	int   doHpDirectorCount = 0;     // HP�𑝂₵����(HP���o�p)
	float hpDirectorElapsedTime = 0.0f;  // HP���o�̌o�ߎ���

	//! HP�_���[�W�֘A(���o�p)
	int   hpDamage = 5;      // HP�_���[�W
	float hpDamageTimer = 2;      // �_���[�W���o���s���܂ł̎���
	float hpDamageDirectorWaitTimer = 0.005f; // HP�����炷���o�Ƀf�B���C��������
	float maxhpDamageDirectorWaitTimer = 0.005f; // �ő�HP���炷�����҂�����
	float deathWaitTimer = 0;      // ���ŃJ�E���g

#endif

//-----------------------------------------------------------------------------------//
	//! HP�̉摜
#if 1
	DirectX::XMFLOAT2 hpSpritePos = { 0.0f, 0.0f };  // HP�Q�[�W�̈ʒu
	DirectX::XMFLOAT3 hpSpriteColor = { 1, 0, 1 };     // HP�Q�[�W�̐F
	DirectX::XMFLOAT2 hpImagePos = { 0.0f, 0.0f };  // HP�摜�̈ʒu
	DirectX::XMFLOAT3 hpImageColor = { 1, 1, 1 };     // HP�摜�̐F

	DirectX::XMFLOAT3 hpSpriteColorP = { 0, 1, 0 }; // HP�Q�[�W�̐F(�v���C���[�p)

	int hpSpriteHeight = 0;     // HP�摜�̍���
	float enemyHpSpriteWidth = 684.5f;  // �G��HP�Q�[�W�̒���
	float playerHpSpriteWidth = 140.01f; // �v���C���[��HP�Q�[�W�̒���

	DirectX::XMFLOAT2 nameSpritePos = { 0, 0 }; // ���O�̉摜�̈ʒu
	float             nameSpriteOpacity = 0.0f;     // ���O�̕s�����x
#endif

//-----------------------------------------------------------------------------------//
	//! HP�V�F�C�N
#if 1
	float playerImageShakePosY = 0.0f; // �v���C���[�摜�V�F�C�N�̈ʒu
	float hpSpriteShakePosY = 0.0f; // HP�Q�[�W�V�F�C�N�̈ʒu
	float hpImageShakePosY = 0.0f; // HP�摜�V�F�C�N�̈ʒu
	bool  hpShake = false;// HP�V�F�C�N���邩
	float hpShakeTimer = 0.0f; // HP�V�F�C�N�^�C�}�[
#endif

//-----------------------------------------------------------------------------------//
	//! HP�`�ԕω�
#if 1
	bool  hpPhaseChange = false; // HP�̌`�ԕω����o
	float hpPhaseChangeTimer = 0.0f;  // HP�̌`�ԕω����o���s������

	//! HP�摜�̐F�ύX
	float hpImageColorMin = 0.2f;                   // HP�摜�̐F(�ŏ��l)
	float hpImageColorMax = 1.0f;                   // HP�摜�̐F(�ő�l)
	DirectX::XMFLOAT3
		hpImageColorChangeSpeed = { 4.0f, 3.8f, 4.4f }; // HP�摜�̐F��ς��鎞��
	struct HpImageColorUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	}hpImageColorUp;    // HP�摜�̐F�𑝂₷��

	//! HP�Q�[�W�̐F�ύX
	float hpSpriteColorMin = 0;     // HP�Q�[�W�̐F(�ŏ��l)
	float hpSpriteColorMax = 1.0f;  // HP�Q�[�W�̐F(�ő�l)
	float hpSpriteColorChangeSpeed = 3.8f;  // HP�Q�[�W�̐F��ς��鎞��
	bool hpSpriteColorDown = false; // HP�Q�[�W�̐F�������邩
#endif

//-----------------------------------------------------------------------------------//
	//! �_���[�W�v�Z�֘A
#if 1
	bool  isGuard = false; // �K�[�h���Ă��邩
	int   damage = 0;     // �_���[�W
	int   additionalDamage = 0;     // �ǉ��_���[�W(�h��͒ቺ�Ȃǂ�\������)
	int   defenses = 0;     // �h���
	float defensesUpTimer = 0.0f;    // �h��̓A�b�v����
#endif

//-----------------------------------------------------------------------------------//

	/*! �G�֘A */
#if 1

	std::unique_ptr<Model> model;     // ���f��
	std::unique_ptr<Model> models[8]; // ���f��(�\����)

	std::unique_ptr<Effect> destroyEffect; // �j���G�t�F�N�g

	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };     // �^�[�Q�b�g�̈ʒu
	float turnSpeed = DirectX::XMConvertToRadians(360); // ��]���x

	DirectX::XMFLOAT3 debugPrimitiveColor = { 0.5f, 0.5f, 0.5f }; // �f�o�b�O�v���~�e�B�u�̐F

	bool isContactDamage = false; // �ڐG�_���[�W�����邩
	int  contactDamage = 12;    // �ڐG�_���[�W�y�Q�[�����Ƀ_���[�W��ς������ꍇ�͈����ɂ��̕ϐ������蓖�Ă�z
	int  contactDamageReset = 12;    // �ڐG�_���[�W�̃��Z�b�g

#endif

	//-----------------------------------------------------------------------------------//
		//! ���̑�
#if 1
	bool  hideSprites = false; // HP�Ȃǂ��B��
	float invincibleTimer = 0.0f;  // ���G����

	float stateChangeWaitTimer = 0.0f; // �X�e�[�g�؂�ւ��܂ł̎���
	float actionTimer = 0.0f; // �A�N�V�����^�C�}�[
	float actionFinishWaitTimer = 0.0f; // �A�N�V�����I���܂ł̑҂�����
	float actionFinishWaitInitialTimer = 0.0f; // �A�N�V�����I���܂ł̑҂����Ԃ̏������
#endif

/*****************************************************************************************************/
	/*! �q�b�g�X�g�b�v */
#if 1
	bool  hitStop = false; // �q�b�g�X�g�b�v����
	bool  hitStopCameraShake = false; // �q�b�g�X�g�b�v�ŃJ������h�炷��
	float hitStopElapsedTime = 1.0f;  // �q�b�g�X�g�b�v���݂�elapsedTime
	float hitStopSeconds = 0.0f;  // �q�b�g�X�g�b�v�̎���
	float hitStopMaxSeconds = 0.3f;  // �q�b�g�X�g�b�v�̍ő�̎���
#endif

	/*****************************************************************************************************/
	float friction = 0.5f; // ���C
	float acceleration = 3.0f; // ������
	float maxMoveSpeed = 5.0f; // �ő�ړ����x
	float moveVecX = 0.0f; // �ړ��x�N�g��X
	float moveVecZ = 0.0f; // �ړ��x�N�g��Z

	float stepOffset = 1.0f; // ���C�L���X�g�̃I�t�Z�b�g
};