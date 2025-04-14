#pragma once

#include <DirectXMath.h>
#include <memory>
#include "Camera.h"

// �O���錾
class Graphics;
class Shader;
class Fade;

// �J�����R���g���[���[
class CameraController
{
public:
	CameraController();
	~CameraController();

	// �B��̃C���X�^���X�擾
	static CameraController& Instance()
	{
		static CameraController cameraController;
		return cameraController;
	}

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void RenderCameraTarget(ID3D11DeviceContext* dc, Shader* shader);

	// �t�F�[�h��`��
	void FadeRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// �f�o�b�O�`��
	void DrawDebugGUI();

	/************************************************************************************************/
		/*! �ǂ̃C�[�W���O�ɂ��邩 */
#if 1
	//! �^�[�Q�b�g
	enum class TargetChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)
	};

	static TargetChangeEasing targetChangeEasing;

	//! �p�x
	enum class AngleChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)
	};

	static AngleChangeEasing angleChangeEasing;

	//! �͈�
	enum class RangeChangeEasing
	{
		Linear,  // ���j�A(�⊮����)
		EaseIn,  // �C�[�Y�C��(����)
		EaseOut, // �C�[�Y�A�E�g(����)
	};

	static RangeChangeEasing rangeChangeEasing;

#endif

	/************************************************************************************************/
		/*! �Z�b�^�[ */

#if 1
	// �^�[�Q�b�g�̈ʒu�ݒ�
	void SetTarget(const DirectX::XMFLOAT3& target)
	{
		//! �f�o�b�O�J��������Ȃ��Ȃ�
		if (!debugCamera)
			this->target = target;
	}

	// �^�[�Q�b�g�̈ʒu�ύX�̐ݒ�
	void SetTargetChange(DirectX::XMFLOAT3 endTargetChange, float targetChangeTime, TargetChangeEasing targetChangeEasing = TargetChangeEasing::Linear)
	{
		if (!targetChange)
		{
			targetChange = true;               // �^�[�Q�b�g�̈ʒu��ς���
			this->startTargetChange = this->target;       // �^�[�Q�b�g�̕ύX�̊J�n�̒l
			this->endTargetChange = endTargetChange;    // �����܂Ń^�[�Q�b�g�̈ʒu��ς���
			this->targetChangeTime = targetChangeTime;   // �^�[�Q�b�g�̈ʒu��ς��鎞��
			this->targetChangeEasing = targetChangeEasing; // �ǂ̃C�[�W���O�ɂ��邩
			this->targetChangeElapsedTime = 0.0f;               // �o�ߎ��Ԃ����Z�b�g
		}
	}

	// �p�x�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle)
	{
		//! �f�o�b�O�J��������Ȃ��Ȃ�
		if (!debugCamera)
			this->angle = angle;
	}

	// �p�x�ύX�̐ݒ�
	void SetAngleChange(DirectX::XMFLOAT3 endAngleChange, float angleChangeTime, AngleChangeEasing angleChangeEasing = AngleChangeEasing::Linear)
	{
		if (!angleChange)
		{
			angleChange = true;              // �p�x��ς���
			this->startAngleChange = this->angle;       // �p�x�̕ύX�̊J�n�̒l
			this->endAngleChange = endAngleChange;    // �����܂Ŋp�x��ς���
			this->angleChangeTime = angleChangeTime;   // �p�x��ς��鎞��
			this->angleChangeEasing = angleChangeEasing; // �ǂ̃C�[�W���O�ɂ��邩
			this->angleChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g
		}
	}

	// �J�����͈̔͂̐ݒ�
	void SetRange(float range)
	{
		//! �f�o�b�O�J��������Ȃ��Ȃ�
		if (!debugCamera)
			this->range = range;
	}

	// �J�����͈̔͂̕ύX�̐ݒ�
	void SetRangeChange(float endRangeChange, float rangeChangeTime, RangeChangeEasing rangeChangeEasing = RangeChangeEasing::Linear)
	{
		if (!rangeChange)
		{
			rangeChange = true;              // �J�����͈̔͂�ς���
			this->startRangeChange = this->range;       // �J�����͈̔͂̕ύX�̊J�n�̒l
			this->endRangeChange = endRangeChange;    // �����܂ŃJ�����͈̔͂�ς���
			this->rangeChangeTime = rangeChangeTime;   // �J�����͈̔͂�ς��鎞��
			this->rangeChangeEasing = rangeChangeEasing; // �ǂ̃C�[�W���O�ɂ��邩
			this->rangeChangeElapsedTime = 0.0f;              // �o�ߎ��Ԃ����Z�b�g
		}
	}

	// �J�����̍����̐ݒ�
	void SetHeight(float height) { this->height = height; }

	// �J�����𒍖ڂ����邩�̐ݒ�
	void SetTracking(bool tracking) { this->tracking = tracking; }

	// �J�������[�r�[���Ԃ̐ݒ�
	void SetCameraMovieTime(float movieTime)
	{
		this->cameraMovie = true;
		this->movieTime = movieTime;
	}

	// �J�����V�F�C�N�ݒ�
	void SetCameraShake(float shakeTime, DirectX::XMINT3 shakePower)
	{
		// �J�����V�F�C�N���ᖳ���Ȃ�ݒ肷��
		this->cameraShake = true;       // �J�����V�F�C�N������
		this->cameraShakeTimer = shakeTime;  // �h�炷����
		this->cameraShakePower = shakePower; // �h�炷�傫��
	}

	// ���̍X�V�������~�߂�ݒ�
	void SetBossFinish()
	{
		this->bossFinishSettings = false; // �ݒ��������
		this->bossFinish = true;  // ���̍X�V�������~�߂�
		this->bossFinishTimer = 1.2f;  // ���̍X�V�������~�߂鎞��
	}

#endif

	/************************************************************************************************/
		/*! �Q�b�^�[ */

		// �J���������[�r�[�����̎擾
	bool GetCameraMovie() { return cameraMovie; }

	// ���̍X�V�������~�߂邩�̎擾
	bool GetBossFinish() { return bossFinish; }

	/************************************************************************************************/

		/*! ���̑��̍X�V���� */

		// �J�����̒��ڍX�V����
	void UpdateCameraTargetTracking(float elapsedTime);

	//-------------------------------------------------------------------//

		// �J�����̏�ԍX�V����
	void UpdateCameraState(float elapsedTime);

	// �J�����̐�������
	void CameraLimit();

	// �f�o�b�O�J����
	bool UpdateDebugCamera(float elapsedTime);

	//---------------------------------------------------------------------------//

		// �J�����̃^�[�Q�b�g�̕ύX�X�V����
	bool UpdateTargetChange(float elapsedTime);

	//---------------------------------------------------------------------------//

		// �J�����̊p�x�̕ύX�X�V����
	bool UpdateAngleChange(float elapsedTime);

	//---------------------------------------------------------------------------//

		// �J�����͈̔͂̕ύX�X�V����
	bool UpdateRangeChange(float elapsedTime);

	// �J�������[�r�[���Ԃ̍X�V����
	bool UpdateCameraMovieTimer(float elapsedTime);

	// �J�����V�F�C�N�X�V����
	bool UpdateCameraShake(float elapsedTime);

	// ���̍X�V�������~�߂�X�V����
	bool UpdateBossFinish(float elapsedTime);

private:

	float rollSpeed = DirectX::XMConvertToRadians(90);
	float minAngleX = DirectX::XMConvertToRadians(-90);
	float maxAngleX = DirectX::XMConvertToRadians(90);

	float height = 9.0f; // �J�����̍���
	float dist = 0.0f; // �v���C���[�ƃ^�[�Q�b�g�̋���

	bool  tracking = false; // �J�������v���C���[���S�Ń^�[�Q�b�g�ɒ��ڂ����邩

	/**********************************************************************************************/
		/*! �^�[�Q�b�g */

public:
	static DirectX::XMFLOAT3 target;

	static bool              targetChange;            // �^�[�Q�b�g�̈ʒu��ς��邩
	static DirectX::XMFLOAT3 startTargetChange;       // �^�[�Q�b�g�̕ύX�̊J�n�̒l
	static DirectX::XMFLOAT3 endTargetChange;         // �����܂Ń^�[�Q�b�g�̈ʒu��ς���
	static float             targetChangeTime;        // �^�[�Q�b�g�̈ʒu��ς��鎞��
	static float             targetChangeElapsedTime; // �^�[�Q�b�g�̈ʒu�ύX�̌o�ߎ���

	/**********************************************************************************************/
		/*! �p�x */

public:
	static DirectX::XMFLOAT3 angle;

	static bool              angleChange;            // �p�x��ς��邩
	static DirectX::XMFLOAT3 startAngleChange;       // �p�x�̕ύX�̊J�n�̒l
	static DirectX::XMFLOAT3 endAngleChange;         // �����܂Ŋp�x��ς���
	static float             angleChangeTime;        // �p�x��ς��鎞��
	static float             angleChangeElapsedTime; // �p�x�ύX�̌o�ߎ���

	/**********************************************************************************************/
		/*! �J�����͈̔� */

private:
	float minRange = 2.0f;   // �J�����͈͂̍ŏ�
	float maxRange = 999.0f; // �J�����͈͂̍ő�

public:
	static float range; // �J�����͈�

	static bool  rangeChange;            // �J�����͈̔͂�ς��邩
	static float startRangeChange;       // �J�����͈̔͂̕ύX�̊J�n�̒l
	static float endRangeChange;         // �����܂ŃJ�����͈̔͂�ς���
	static float rangeChangeTime;        // �J�����͈̔͂�ς��鎞��
	static float rangeChangeElapsedTime; // �J�����͈͕̔ύX�̌o�ߎ���

	/**********************************************************************************************/
		/*! �J�����V�F�C�N */

public:
	static bool  cameraShake;                // �J������h�炷��
	static float cameraShakeTimer;           // �h�炷����
	static DirectX::XMINT3 cameraShakePower; // �h�炷�傫��

	/**********************************************************************************************/
public:
	static bool debugCamera; // �f�o�b�O�J����
	static bool noMoveDebugCamera; // �f�o�b�O�J�����̓������~�߂邩
private:
	float mouseMoveSpeed = 1.5f; // �}�E�X�̐i�ޑ���
	float mouseZoomSpeed = 0.1f; // �}�E�X�̊g�呬�x
	float mouseSensitivity = 1.0f; // �}�E�X���x

	float targetMoveSpeed = 8; // �^�[�Q�b�g�̈ړ����x(�{�^���ړ��̏ꍇ)
	float targetUpSpeed = 8;  // �^�[�Q�b�g�̏㏸���x(�{�^���ړ��̏ꍇ)

	//-----------------------------------------------------------------//

	bool cameraTargetPlayer_3D = false; // �J�����̃^�[�Q�b�g���v���C���[�ɂ��邩

	bool  cameraMovie = false; // �J�����̃��[�r�[����
	float movieTime = 0.0f;  // ���[�r�[�̎���

	//-----------------------------------------------------------------//

	std::unique_ptr<Fade> fade;
	bool bossFinishSettings = false; // �ݒ肵����

	bool  bossFinish = false; // �{�X�t�B�j�b�V�����邩
	float bossFinishTimer = 0.0f;  // �{�X�t�B�j�b�V������
};