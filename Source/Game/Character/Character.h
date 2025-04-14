#pragma once

#include<DirectXMath.h>
#include <memory>

// �L�����N�^�[
class Character : public std::enable_shared_from_this<Character>
{
public:
	Character() {}
	virtual ~Character() {}

	// �����蔻��̈ʒu��ݒ�
	void CollisionPosSettings();

	// �s��X�V����
	void UpdateTransform();

	// �n�ʂɐڒn���Ă��邩
	bool IsGround() const { return isGround; }

	// �_���[�W��^����
	bool ApplyDamage(int damage, float invincibleTime, float defensesUpTime);

	/**************************************************************************************/
		/*! �ǂ̃C�[�W���O�ɂ��邩 */
#if 1
	//! �ʒu
	enum class PositionChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)

	}positionChangeEasing;

	//! �X�P�[��
	enum class ScaleChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)

	}scaleChangeEasing;

	//! �p�x
	enum class AngleChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)

	}angleChangeEasing;
#endif

	/**************************************************************************************/
		/*! �Z�b�^�[ */
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

	//---------------------------------------------------------------------------//

		// �}�e���A���̐F�̐ݒ�
	void SetMaterialColor(const DirectX::XMFLOAT3 materialColor) { this->materialColor = materialColor; }

	// �}�e���A���̐F�ύX�̐ݒ�
	void SetMaterialColorChange(DirectX::XMFLOAT3 endMaterialColorChange, DirectX::XMFLOAT3 materialColorChangeTime)
	{
		if (!this->materialColorChange)
		{
			this->materialColorChange = true;                    // �}�e���A���̐F��ς���
			this->startMaterialColorChange = this->materialColor;     // �}�e���A���̐F�̕ύX�̊J�n�̒l
			this->endMaterialColorChange = endMaterialColorChange;  // �����܂Ń}�e���A���̐F��ς���
			this->materialColorChangeTime = materialColorChangeTime; // �}�e���A���̐F��ς��鎞��
			this->materialColorChangeElapsedTime = 0.0f;                    // �҂����Ԃ����Z�b�g
		}
	}

	// �s�����x��ݒ�
	void SetOpacity(float opacity) { this->opacity = opacity; }

	// �s�����x�ύX�����̐ݒ�
	void SetOpacityChange(float endOpacityChange, float opacityChangeTime)
	{
		if (!this->opacityChange)
		{
			this->opacityChange = true;              // �s�����x��ς���
			this->startOpacityChange = this->opacity;     // �s�����x�̕ύX�̊J�n�̒l
			this->endOpacityChange = endOpacityChange;  // �����܂ŕs�����x��ς���
			this->opacityChangeTime = opacityChangeTime; // �s�����x��ς��鎞��
			this->opacityChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g
		}
	}

	//---------------------------------------------------------------------------//

		// �ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// �ʒu�ύX�����̉���(�o�O�h�~�p)
	void SetPositionChangeUnlock() { this->positionChange = false; }

	// �ʒu�ύX�̐ݒ�
	void SetPositionChange(const DirectX::XMFLOAT3& endPositionChange,
		const DirectX::XMFLOAT3& positionChangeTime, PositionChangeEasing positionChangeEasing = PositionChangeEasing::Linear)
	{
		if (!positionChange)
		{
			this->positionChange = true;                 // �ʒu��ς���
			this->startPositionChange = this->position;       // �ʒu�̕ύX�̊J�n�̒l
			this->endPositionChange = endPositionChange;    // �����܂ňʒu��ς���
			this->positionChangeTime = positionChangeTime;   // �ʒu��ς��鎞��
			this->positionChangeEasing = positionChangeEasing; // ���̃C�[�W���O�ɂ��邩
			this->positionChangeElapsedTime = 0.0f;                 // �o�ߎ��Ԃ����Z�b�g
		}
	}

	// �d�͐ݒ�
	void SetGravity(float gravity) { this->gravity = gravity; }

	// �����x�ݒ�
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	// �����蔻��̈ʒu�����̐ݒ�
	void SetCollisionOffset(const DirectX::XMFLOAT3& collisionOffset) { this->collisionOffset = collisionOffset; }

	//---------------------------------------------------------------------------//

		// �p�x�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// �p�x�ύX�̐ݒ�
	void SetAngleChange(const DirectX::XMFLOAT3& endAngleChange,
		const DirectX::XMFLOAT3& angleChangeTime, AngleChangeEasing angleChangeEasing = AngleChangeEasing::Linear)
	{
		if (!this->angleChange)
		{
			this->angleChange = true;              // �p�x��ς���
			this->startAngleChange = this->angle;       // �p�x�̕ύX�̊J�n�̒l
			this->endAngleChange = endAngleChange;    // �����܂Ŋp�x��ς���
			this->angleChangeTime = angleChangeTime;   // �p�x��ς��鎞��
			this->angleChangeEasing = angleChangeEasing; // �ǂ̃C�[�W���O�ɂ��邩
			this->angleChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g
		}
	}

	// �p�x�̉�]�̐ݒ�(��]���x�̌���(�})�͎蓮)
	void SetAngleRotation(float rotationTime, const DirectX::XMFLOAT3& angleRotationSpeed)
	{
		if (!this->angleRotation)
		{
			this->angleRotation = true;               // �p�x����]������
			this->rotationTime = rotationTime;       // ��]����
			this->angleRotationSpeed = angleRotationSpeed; // �p�x�̉�]���x
		}
	}

	//---------------------------------------------------------------------------//

		// �X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// �X�P�[���ύX�̐ݒ�
	void SetScaleChange(const DirectX::XMFLOAT3& endScaleChange,
		const DirectX::XMFLOAT3& scaleChangeTime, ScaleChangeEasing scaleChangeEasing = ScaleChangeEasing::Linear)
	{
		if (!this->scaleChange)
		{
			this->scaleChange = true;              // �X�P�[����ς���
			this->startScaleChange = this->scale;       // �X�P�[���̕ύX�̊J�n�̒l
			this->endScaleChange = endScaleChange;    // �����܂ŃX�P�[����ς���
			this->scaleChangeTime = scaleChangeTime;   // �X�P�[����ς��鎞��
			this->scaleChangeEasing = scaleChangeEasing; // �ǂ̃C�[�W���O�ɂ��邩
			this->scaleChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g
		}
	}

	//---------------------------------------------------------------------------//

		// ���[�r�[���̃A�j���[�V�����ݒ�
	void SetMovieAnimation(int movieAnimNum, bool movieAnimLoop)
	{
		this->movieAnimation = false;         // �A�j���[�V������ݒ�ł���悤�ɂ���
		this->movieAnimNum = movieAnimNum;  // �A�j���[�V�����̔ԍ�
		this->movieAnimLoop = movieAnimLoop; // �A�j���[�V���������[�v�����邩
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

	// �A�N�V�������I������܂ł̑҂����Ԃ�ݒ�
	void SetActionFinishWaitTimer(float actionFinishWaitTimer) { this->actionFinishWaitTimer = actionFinishWaitTimer; }

	// ���G���Ԑݒ�
	void SetInvincibleTimer(float invincibleTimer) { this->invincibleTimer = invincibleTimer; }

	//---------------------------------------------------------------------------//

		// �X�v���C�g��\���ݒ�
	void SetHideSprites(bool hideSprites) { this->hideSprites = hideSprites; }

	//---------------------------------------------------------------------------//

		// ���a�ݒ�
	void SetRadius(float radius) { this->radius = radius; }

	// �����ݒ�
	void SetHeight(float height) { this->height = height; }

	//---------------------------------------------------------------------------//
		/*! �J���� */
#if 1
	// �J�����̃^�[�Q�b�g�̈ʒu�ύX�̐ݒ�
	void SetCameraTargetChange(DirectX::XMFLOAT3 endCameraTarget, float cameraTargetChangeTime, int targetChangeEasing = 0);

	// �J�����̊p�x�ύX�̐ݒ�
	void SetCameraAngleChange(DirectX::XMFLOAT3 endCameraAngle, float cameraAngleChangeTime, int = 0);

	// �J�����͈̔͂̕ύX�̐ݒ�
	void SetCameraRangeChange(float endCameraRange, float cameraRangeChangeTime, int rangeChangeEasing = 0);

	// �J�����V�F�C�N�ݒ�
	void SetCameraShake(float cameraShakeTimer, const DirectX::XMINT3& cameraShakePower);
#endif

	//---------------------------------------------------------------------------//
		/*! �V�F�[�_�[ */

		//! �|�X�g�G�t�F�N�g
#if 1
	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
	void SetPostEffectStatusOnce(float contrast = 1.0f,
		float saturation = 0.8f,
		const DirectX::XMFLOAT3 colorFilter = { 1.2f, 1.3f, 1.35f },
		float chromaticAberration = 0);

	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
	void SetPostEffectStatusOnceResetChange(float endContrastChange = 1.0f,
		float contrastChangeTime = 1.5f,
		float endSaturationChange = 0.8f,
		float saturationChangeTime = 1.5f,
		const DirectX::XMFLOAT3 endColorFilterChange = { 1.2f, 1.3f, 1.35f },
		float colorFilterChangeTime = 1.5f,
		float endChromaticAberrationChange = 0,
		float chromaticAberrationChangeTime = 1.5f);

	// �R���g���X�g�ύX�̐ݒ�
	void SetContrastChange(float endContrastChange, float contrastChangeTime = 1.5f);

	// �T�`�����[�V����(�F�̍ʓx)�ύX�̐ݒ�
	void SetSaturationChange(float endSaturationChange, float saturationChange = 1.5f);

	// �J���[�t�B���^�[�ύX�̐ݒ�
	void SetColorFilterChange(DirectX::XMFLOAT3& endColorFilterChange, float colorFilterChangeTime = 1.5f);

	// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�ύX�̐ݒ�
	void SetChromaticAberrationChange(float endChromaticAberrationChange, float chromaticAberrationChangeTime = 1.5f);

#endif

	// �G�~�b�V�u�̐F�̐ݒ�
	void SetEmissiveColor(const DirectX::XMFLOAT3 emissiveColor) { this->emissiveColor = emissiveColor; }

	// �G�~�b�V�u�̐F�ύX�̐ݒ�
	void SetEmissiveColorChange(DirectX::XMFLOAT3 endEmissiveColorChange, DirectX::XMFLOAT3 emissiveColorChangeTime)
	{
		if (!this->emissiveColorChange)
		{
			this->emissiveColorChange = true;                    //�G�~�b�V�u�̐F��ς���
			this->startEmissiveColorChange = this->emissiveColor;     // �G�~�b�V�u�̋����̕ύX�̊J�n�̒l
			this->endEmissiveColorChange = endEmissiveColorChange;  // �����܂ŃG�~�V�b�u�̐F��ς���
			this->emissiveColorChangeTime = emissiveColorChangeTime; //�G�~�b�V�u�̐F��ς��鎞��
			this->emissiveColorChangeElapsedTime = 0.0f;                    // �o�ߎ��Ԃ����Z�b�g
		}
	}

	// �G�~�b�V�u�̋����̐ݒ�
	void SetEmissiveStrength(float emissiveStrength) { this->emissiveStrength = emissiveStrength; }

	// �G�~�b�V�u�̋����ύX�̐ݒ�(�ݒ�̎��ɐF��ς����)
	void SetEmissiveStrengthChange(DirectX::XMFLOAT3 emissiveColor, float endEmissiveStrengthChange, float emissiveStrengthChangeTime)
	{
		if (!this->emissiveStrengthChange)
		{
			this->emissiveColor = emissiveColor;              //�G�~�b�V�u�̐F
			this->emissiveStrengthChange = true;                       //�G�~�b�V�u�̋�����ς���
			this->startEmissiveStrengthChange = this->emissiveStrength;     // �G�~�b�V�u�̋����̕ύX�̊J�n�̒l
			this->endEmissiveStrengthChange = endEmissiveStrengthChange;  // �����܂ŃG�~�b�V�u�̋�����ς���
			this->emissiveStrengthChangeTime = emissiveStrengthChangeTime; // �G�~�b�V�u�̋�����ς��鎞��
			this->emissiveStrengthChangeElapsedTime = 0.0f;                       // �o�ߎ��Ԃ����Z�b�g
		}
	}

	// �G�~�b�V�u�̋����ύX�̉���
	void SetEmissiveStrengthChangeUnlock() { emissiveStrengthChange = false; }

	// �G�~�b�V�u�̌`�ԕω����o�̐ݒ�
	void SetEmissivePhaseChange(float emissiveTimerPhaseChangeTimer, float emissiveStrength)
	{
		if (!emissivePhaseChange)
		{
			emissivePhaseChange = true;                          //�G�~�b�V�u�̌`�ԕω����o������
			this->emissivePhaseChangeTimer = emissiveTimerPhaseChangeTimer; //�G�~�b�V�u�̌`�ԕω����o�����鎞��
			this->emissiveStrength = emissiveStrength;              //�G�~�b�V�u�̋���
		}
	}

#endif

	/**************************************************************************************/
		/*! �Q�b�^�[ */
#if 1

	// HP���擾
	int GetHp() const { return hp; }

	// HP�_���[�W�擾
	int GetHpDamage() const { return hpDamage; }

	// �ő�HP���擾
	int GetMaxHp() const { return maxHp; }

	//---------------------------------------------------------------------------//

		// �X�e�[�g�ύX�܂ł̎��Ԃ��擾
	float GetStateChangeWaitTimer()  const { return stateChangeWaitTimer; }

	// �A�N�V�����^�C�}�[���擾
	float GetActionTimer() const { return actionTimer; }

	// �A�N�V�����I���܂ł̑҂����Ԃ��擾
	float GetActionFinishWaitTimer() const { return actionFinishWaitTimer; }

	// �q�b�g�X�g�b�v���Ԃ̎擾
	float GetHitStopElapsedTime() { return hitStopElapsedTime; }

	// ���G���Ԏ擾
	float GetInvincibleTimer() const { return invincibleTimer; }

	// �h��̓A�b�v���Ԏ擾
	float GetDefensesUpTimer() const { return defensesUpTimer; }

	// HP�����I���擾
	bool GetHpDirectorFinished() const { return hpDirectorFinished; }

	//---------------------------------------------------------------------------//

		// ���[�r�[�����ǂ������擾
	bool GetMovieScene() const { return movieScene; }

	// ���[�r�[���Ԏ擾
	float GetMovieTime() const { return movieTime; }

	//---------------------------------------------------------------------------//

		// �s�����x��ς��Ă��邩���擾
	bool GetOpacityChange() { return opacityChange; }

	// �G�~�b�V�u�̐F��ς��Ă��邩���擾
	bool GetEmissiveColorChange() { return emissiveColorChange; }

	// �G�~�b�V�u�̋�����ς��Ă��邩���擾
	bool GetEmissiveStrengthChange() { return emissiveStrengthChange; }

	//---------------------------------------------------------------------------//

		// �g�����X�t�H�[���擾
	const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	// �ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// �ʒu�ύX�����Ă��邩���擾
	bool GetPositionChange() { return positionChange; }

	// �����蔻��̈ʒu�擾
	const DirectX::XMFLOAT3& GetCollisionPos() const { return collisionPos; }

	// �����蔻��̒������擾
	const DirectX::XMFLOAT3& GetCollisionOffset() const { return collisionOffset; }

	// �����x�擾
	const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }

	//---------------------------------------------------------------------------//

		// �p�x�擾
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	// �p�x�ύX�����Ă��邩���擾
	bool GetAngleChange() { return angleChange; }

	//---------------------------------------------------------------------------//

		// �X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	// �X�P�[���ύX�����Ă��邩���擾
	bool GetScaleChange() { return scaleChange; }

	//---------------------------------------------------------------------------//

		// ���a�擾
	float GetRadius() { return radius; }

	// �����擾
	float GetHeight() { return height; }

#endif

protected:

	/*! HP�֘A */
#if 1

	// HP�Ǘ�
	void HpControll(float elapsedTime);

	// HP���o
	bool HpDirector(int hpPlusNum = 0, int doNum = 0);

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

	/**************************************************************************************/
		/*! ���͏��� */
#if 1
	// ���͏����X�V
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

	/**************************************************************************************/

		/*! �ړ��֘A */
#if 1

	// �X�e�B�b�N���͒l����ړ��x�N�g�����K��(3D)
	DirectX::XMFLOAT3 GetMoveVec() const;

	// �X�e�B�b�N���͒l����ړ��x�N�g�����K��(2D)
	DirectX::XMFLOAT3 GetMoveVec_2D() const;

	// �ړ�����(3D)
	void Move3D(float vx, float vz, float speed);

	// �ړ�����(2D)
	void Move2D(float vx, float speed);

	// ���񏈗�(�[���x�N�g���ł��������~�߂Ȃ�)
	void Turn3DNotCancel(float elapsedTime, float vx, float vz, float speed);

	// ���񏈗�(3D)
	void Turn3D(float elapsedTime, float vx, float vz, float speed);

	// ���񏈗�(2D)
	void Turn2D(float elapsedTime, float vx, float speed);

	// �W�����v����
	void Jump(float speed);

#endif

	/**************************************************************************************/

		// ��ԍX�V����
	void UpdateCharacterState(float elapsedTime);

	// ��ԍX�V�����֘A(��̊֐��ɂ܂Ƃ߂��Ă���)
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

	//---------------------------------------------------------------------------//

	// �ʒu�ύX�X�V����
	bool UpdatePositionChange(float elapsedTime);

	//---------------------------------------------------------------------------//

	// �p�x��������
	void LimitAngle();

	// ��]�ύX�X�V����
	bool UpdateAngleChange(float elapsedTime);

	// �p�x�̉�]�̍X�V����(����̒l�͖ڎw�����A�w�肳�ꂽ���Ԓ��ɉ�]����)
	bool UpdateAngleRotation(float elapsedTime);

	//---------------------------------------------------------------------------//

	// �X�P�[���ύX�X�V����
	bool UpdateScaleChange(float elapsedTime);


	//---------------------------------------------------------------------------//

	// ���[�r�[���ԍX�V����
	bool UpdateMovieTimer(float elapsedTime);

	// �q�b�g�X�g�b�v�X�V����
	bool UpdateHitStop(float elapsedTime);

	// ���G���ԍX�V
	void UpdateInvincibleTimer(float elapsedTime);

	// �h��̓A�b�v���ԍX�V
	void UpdateDefensesUpTimer(float elapsedTime);

	//---------------------------------------------------------------------------//

#endif

	// ���n�������ɌĂ΂��
	virtual void OnLanding() {}

	// �_���[�W���󂯂����ɌĂ΂��
	virtual void OnDamaged() {};

	// ���S�������ɌĂ΂��
	virtual void OnDead();

protected:
	DirectX::XMFLOAT3    collisionPos = { 0, 0, 0 };
	DirectX::XMFLOAT3    collisionOffset = { 0, 0, 0 };
	DirectX::XMFLOAT4X4  transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	// �~���Ȃǂ̓����蔻��̑傫��
	float radius = 0.45f; // ���a
	float height = 1.4f;  // ����

	float gravity = 0.5f; // �d��

	bool isGround = false; // �n�ʂɂ���

	float moveSpeed = 50.0f; // �ړ����x

/*****************************************************************************************************/
	/*! �}�e���A�� */
	DirectX::XMFLOAT3 materialColor = { 1,1,1 }; // �}�e���A���̐F
	bool materialColorChange = false;     // �}�e���A���̐F��ύX���邩

	DirectX::XMFLOAT3 startMaterialColorChange = { 0, 0, 0 }; // �}�e���A���̐F�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endMaterialColorChange = { 0, 0, 0 }; // �����܂Ń}�e���A���̐F��ς���
	DirectX::XMFLOAT3 materialColorChangeTime = { 0, 0, 0 }; // �}�e���A���̐F��ς��鎞��
	float materialColorChangeElapsedTime = 0.0f;        // �}�e���A���̐F�ύX�̌o�ߎ���

	//! �s�����x
	float opacity = 1.0f;  // �s�����x
	bool  opacityChange = false; // �s�����x��ύX���邩

	float startOpacityChange = 0.0f;  // �s�����x�̕ύX�̊J�n�̒l
	float endOpacityChange = 0.0f;  // �����܂ŕs�����x��ς��� 
	float opacityChangeTime = 0.0f;  // �s�����x��ς��鎞��
	float opacityChangeElapsedTime = 0.0f;  // �s�����x�ύX�̌o�ߎ���

	/*****************************************************************************************************/
		/*! �G�~�b�V�u */
	DirectX::XMFLOAT3 emissiveColor = { 0.0f, 0.0f, 0.0f }; //�G�~�b�V�u�J���[
	bool emissiveColorChange = false;                //�G�~�b�V�u�̐F��ς��邩

	DirectX::XMFLOAT3 startEmissiveColorChange = { 0, 0, 0 }; // �G�~�V�b�u�̐F�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endEmissiveColorChange = { 0, 0, 0 }; // �����܂ŃG�~�V�b�u�̐F��ς���
	DirectX::XMFLOAT3 emissiveColorChangeTime = { 0, 0, 0 }; // �G�~�b�V�u�̐F��ς��鎞��
	float emissiveColorChangeElapsedTime = 0.0f;        // �G�~�b�V�u�̐F�ύX�̌o�ߎ���

	//! �G�~�b�V�u�̋���
	float emissiveStrength = 0.0f;  // �G�~�b�V�u�̋���
	bool  emissiveStrengthChange = false; // �G�~�b�V�u�̋�����ς��邩

	float startEmissiveStrengthChange = 0.0f; // �G�~�b�V�u�̋����̕ύX�̊J�n�̒l
	float endEmissiveStrengthChange = 0.0f; // �����܂ŃG�~�V�b�u�̋�����ς���
	float emissiveStrengthChangeTime = 0.0f; // �G�~�V�b�u�̋�����ς��鎞��
	float emissiveStrengthChangeElapsedTime = 0.0f; // �G�~�b�V�u�̋����ύX�̌o�ߎ���

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
		/*! �ʒu */
#if 1

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	bool positionChange = false; // �ʒu��ύX���邩

	DirectX::XMFLOAT3 startPositionChange = { 0, 0, 0 }; // �ʒu�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endPositionChange = { 0, 0, 0 }; // �����܂ňʒu��ς���
	DirectX::XMFLOAT3 positionChangeTime = { 0, 0, 0 }; // �ʒu��ς��鎞��
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 }; // �����x
	float positionChangeElapsedTime = 0.0f;        // �ʒu�ύX�̌o�ߎ���
#endif

	/*****************************************************************************************************/
		/*! �p�x */
#if 1

	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	bool angleChange = false; // �p�x��ύX���邩

	DirectX::XMFLOAT3 startAngleChange = { 0, 0, 0 }; // �p�x�̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endAngleChange = { 0, 0, 0 }; // �����܂Ŋp�x��ς���
	DirectX::XMFLOAT3 angleChangeTime = { 0, 0, 0 }; // �p�x��ς��鎞��
	float angleChangeElapsedTime = 0.0f;        // �p�x�ύX�̌o�ߎ���

	//! ��](����̒l�͖ڎw���Ȃ�)
	float angleRotation = false; // �p�x����]�����邩
	float rotationTime = 0.0f;  // ��]����
	DirectX::XMFLOAT3 angleRotationSpeed = { 0, 0, 0 }; // �p�x�̉�]���x

#endif

	/*****************************************************************************************************/
		/*! �X�P�[�� */
#if 1 

	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	bool scaleChange = false;// �X�P�[����ύX���邩

	DirectX::XMFLOAT3 startScaleChange = { 0, 0, 0 }; // �X�P�[���̕ύX�̊J�n�̒l
	DirectX::XMFLOAT3 endScaleChange = { 0, 0, 0 }; // �����܂ŃX�P�[����ς���
	DirectX::XMFLOAT3 scaleChangeTime = { 0, 0, 0 }; // �X�P�[����ς��鎞��
	float scaleChangeElapsedTime = 0.0f;        // �X�P�[���ύX�̌o�ߎ���

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

	int  hp = 10;    // HP
	int  maxHp = 5;    // �ő�HP
	bool hpDirectorFinished = false; // HP�̉��o���I�������
	int  doHpDirectorCount = 0;     // HP�𑝂₵����(HP���o�p)

	//! HP�_���[�W�֘A(���o�p)
	int   hpDamage = 5; // HP�_���[�W
	float hpDamageCount = 2; // �_���[�W���o���s���܂ł̎���
	float hpDamageDirectorWaitCount = 0; // HP�����炷���o�Ƀf�B���C��������
	float maxHpDamageDirectorWaitCount = 0; // �ő�HP���炷�����҂�����
	float deathWaitTimer = 0; // ���ŃJ�E���g

	float nameSpriteOpacity = 0.0f; // ���O�̕s�����x
	int hpSpriteHeight = 0;    // HP�摜�̍���

#endif

	//-----------------------------------------------------------------------------------//
		//! HP�̉摜
#if 1
	DirectX::XMFLOAT2 hpSpritePos = { 0.0f, 0.0f };  // HP�Q�[�W�̈ʒu
	DirectX::XMFLOAT3 hpSpriteColor = { 1, 0, 1 };     // HP�Q�[�W�̐F
	DirectX::XMFLOAT2 hpImagePos = { 0.0f, 0.0f };  // HP�摜�̈ʒu
	DirectX::XMFLOAT3 hpImageColor = { 1, 1, 1 };     // HP�摜�̐F

	DirectX::XMFLOAT3 hpSpriteColorP = { 0, 1, 0 }; // HP�Q�[�W�̐F(�v���C���[�p)
#endif

	//-----------------------------------------------------------------------------------//
		//! HP�V�F�C�N
#if 1
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
		//! ���̑�
	bool  hideSprites = false; // HP�Ȃǂ��B��
	float invincibleTimer = 0.0f;  // ���G����

	float stateChangeWaitTimer = 0.0f; // �X�e�[�g�؂�ւ��܂ł̎���
	float actionTimer = 0.0f; // �A�N�V�����^�C�}�[
	float actionFinishWaitTimer = 0.0f; // �A�N�V�����I���܂ł̑҂�����

	/*****************************************************************************************************/
		/*! �q�b�g�X�g�b�v */

	bool  hitStop = false; // �q�b�g�X�g�b�v����
	float hitStopElapsedTime = 1.0f;  // �q�b�g�X�g�b�v���݂�elapsedTime
	float hitStopSeconds = 0.0f;  // �q�b�g�X�g�b�v�̎���
	float hitStopMaxSeconds = 0.3f;  // �q�b�g�X�g�b�v�̍ő�̎���

	/*****************************************************************************************************/
	float friction = 0.5f; // ���C
	float acceleration = 3.0f; // ������
	float maxMoveSpeed = 5.0f; // �ő�ړ����x
	float moveVecX = 0.0f; // �ړ��x�N�g��X
	float moveVecZ = 0.0f; // �ړ��x�N�g��Z

	float stepOffset = 1.0f; // ���C�L���X�g�̃I�t�Z�b�g
};