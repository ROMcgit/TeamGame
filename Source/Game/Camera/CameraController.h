#pragma once

#include <DirectXMath.h>
#include <memory>
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "Game/Character/Director/SettingsDirector/DirectorManager.h"

// �O���錾
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
	void RenderTarget(ID3D11DeviceContext* dc, Shader* shader);

	// �t�F�[�h��`��
	void FadeRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// �f�o�b�O�`��
	void DrawDebugGUI();

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
	void SetTargetChange(DirectX::XMFLOAT3 toTargetChange, DirectX::XMFLOAT3 targetChangeSpeed)
	{
		if (!targetChange)
		{
			targetChange = true;              // �^�[�Q�b�g�̈ʒu��ς���
			this->toTargetChange = toTargetChange;          // �����܂Ń^�[�Q�b�g�̈ʒu��ς���
			this->targetChangeSpeed = targetChangeSpeed; // �^�[�Q�b�g�̈ʒu��ς��鑬��

			// �^�[�Q�b�gX���ڎw���ʒu��菬������
			if (target.x < toTargetChange.x)
				targetChangeUp.x = true;
			else
				targetChangeUp.x = false;

			// �^�[�Q�b�gY���ڎw���ʒu��菬������
			if (target.y < toTargetChange.y)
				targetChangeUp.y = true;
			else
				targetChangeUp.y = false;

			// �^�[�Q�b�gZ���ڎw���ʒu��菬������
			if (target.z < toTargetChange.z)
				targetChangeUp.z = true;
			else
				targetChangeUp.z = false;
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
	void SetAngleChange(DirectX::XMFLOAT3 toAngleChange, DirectX::XMFLOAT3 angleChangeSpeed)
	{
		if (!angleChange)
		{
			angleChange = true;             // �p�x��ς���
			this->toAngleChange = toAngleChange;          // �����܂Ŋp�x��ς���
			this->angleChangeSpeed = angleChangeSpeed; // �p�x��ς��鑬��

			// �p�xX���ڎw���ʒu��菬������
			if (angle.x < toAngleChange.x)
				angleChangeUp.x = true;
			else
				angleChangeUp.x = false;

			// �p�xY���ڎw���ʒu��菬������
			if (angle.y < toAngleChange.y)
				angleChangeUp.y = true;
			else
				angleChangeUp.y = false;

			// �p�xZ���ڎw���ʒu��菬������
			if (angle.z < toAngleChange.z)
				angleChangeUp.z = true;
			else
				angleChangeUp.z = false;
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
	void SetRangeChange(float toRangeChange, float rangeChangeSpeed)
	{
		if (!rangeChange)
		{
			rangeChange = true;             // �J�����͈̔͂�ς���
			this->toRangeChange = toRangeChange;          // �����܂ŃJ�����͈̔͂�ς���
			this->rangeChangeSpeed = rangeChangeSpeed; // �J�����͈̔͂�ς��鑬��

			// �J�����͈̔͂��ڎw���傫����菬������
			if (range < toRangeChange)
				rangeChangeUp = true;
			else
				rangeChangeUp = false;
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

		// �J�����̃^�[�Q�b�g�X�V����
	void UpdateCameraTarget(float elapsedTime);

	//-------------------------------------------------------------------//

		// �J�����̏�ԍX�V����
	void UpdateCameraState(float elapsedTime);

	// �J�����̐�������
	void CameraLimit();

	// �f�u�b�O�J����
	bool UpdateDebugCamera(float elapsedTime);

	//---------------------------------------------------------------------------//

		// �J�����̃^�[�Q�b�g�̕ύX�X�V����
	bool UpdateTargetChange(float elapsedTime);

	// �P�ꎲ�̃^�[�Q�b�g�ʒu�ύX����
	float UpdateTargetAxis(float target, float speed, bool targetChangeUp, float toTargetChangeChange, float elapsedTime);

	//---------------------------------------------------------------------------//

		// �J�����̊p�x�̕ύX�X�V����
	bool UpdateAngleChange(float elapsedTime);

	// �P�ꎲ�̉�]�ύX����
	float UpdateAngleAxis(float angle, float speed, bool angleChangeUp, float toAngleChange, float elapsedTime);

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
	DirectorManager directorManager;

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

	static bool              targetChange;      // �^�[�Q�b�g�̈ʒu��ς��邩
	static DirectX::XMFLOAT3 toTargetChange;    // �����܂Ń^�[�Q�b�g�̈ʒu��ς���
	static DirectX::XMFLOAT3 targetChangeSpeed; // �^�[�Q�b�g�̈ʒu��ς��鑬��

	struct TargetChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};
	static TargetChangeUp targetChangeUp; // �^�[�Q�b�g�̈ʒu�𑝂₷��

	/**********************************************************************************************/
		/*! �p�x */

public:
	static DirectX::XMFLOAT3 angle;

	static bool              angleChange;      // �p�x��ς��邩
	static DirectX::XMFLOAT3 toAngleChange;    // �����܂Ŋp�x��ς���
	static DirectX::XMFLOAT3 angleChangeSpeed; // �p�x��ς��鑬��

	struct AngleChangeUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};
	static AngleChangeUp angleChangeUp; // �p�x�𑝂₷��

	/**********************************************************************************************/
		/*! �J�����͈̔� */

private:
	float minRange = 2.0f;   // �J�����͈͂̍ŏ�
	float maxRange = 999.0f; // �J�����͈͂̍ő�

public:
	static float range; // �J�����͈�

	static bool  rangeChange;      // �J�����͈̔͂�ς��邩
	static float toRangeChange;    // �����܂ŃJ�����͈̔͂�ς���
	static float rangeChangeSpeed; // �J�����͈̔͂�ς��鑬��
	static bool  rangeChangeUp;    // �J�����͈̔͂𑝂₷��

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

	bool bossFinish = false; // �{�X�t�B�j�b�V�����邩
	float bossFinishTimer = 0.0f;  // �{�X�t�B�j�b�V������
};