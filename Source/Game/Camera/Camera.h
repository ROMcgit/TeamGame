#pragma once

#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>

// �J����
class Camera
{
public:
	//! �|�X�g�G�t�F�N�g�𐶐�
	void CreatePostEffect();

	//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̐ݒ�(��񂾂�)�t���O��������
	void SetPostEffectStatusOnceUnlock() { this->setPostEffectStatusOnce = false; }

	//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̐ݒ�(��񂾂�)
	void SetPostEffectStatusOnce();

	//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
	void SetPostEffectStatus();
public:
	//! �|�X�g�G�t�F�N�g�֘A
	struct CBPostEffectParam
	{
		float contrast;            // �R���X�g���N�g(���l���傫���قǁA���邢�����͂�薾�邭�A�Â������͂��Â��Ȃ�)
		float saturation;          // �T�`�����[�V����(�F�̍ʓx)
		float chromaticAberration; // �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))
		float hueShift;            // �F���V�t�g�i-1.0 �` 1.0�j

		DirectX::XMFLOAT3 colorFilter; // �J���[�t�B���^�[(�F�t�B���^�[)
		float dummy1;                  // 4��؂�p

		float vignetteIntensity; // �r�l�b�g�̋��x
		float vignetteOpacity;   // �r�l�b�g�̕s�����x
		float blurStrength;      // �u���[�̋��x
		float dummy2;            // 4��؂�p

		float bloomThreshold;     // �O���A��臒l
		float bloomIntensity;     // �O���A�̋��x
		DirectX::XMFLOAT2 dummy3; // 4��؂�p

		float focusDistance;   // �s���g�����킹�鋗��
		float focusRange;      // �s���g�������͈�
		float dofBlurStrength; // ��ʊE�[�x�̃u���[���x
		float dummy4;          // 4��؂�p

		DirectX::XMFLOAT2 shakeOffset; // �V�F�C�N�̃I�t�Z�b�g
		float shakeStrength;           // �V�F�C�N�̋��x
		float shakeTime;               // �O������n������
		float shakeSpeed;              // �V�F�C�N�̑��x
		DirectX::XMFLOAT3 dummy5;      // 4��؂�p
	};
	static CBPostEffectParam postEffect;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CBPostEffect;

public:
	Camera();
	~Camera() {}

public:
	// �B��̃C���X�^���X�擾
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}

	// �X�V����
	void Update(float elapsedTime);

	// �f�o�b�OGUI�`��
	void DrawDebugGUI();

	/****************************************************************************************/
		/*! �Z�b�^�[ */

		// �w�����������
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
		const DirectX::XMFLOAT3& up);

	// �p�[�X�y�N�e�B�u�ݒ�
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// �J�����V�F�C�N(�V�F�[�_�[)�̐ݒ�
	void SetCameraShakeShader(float cameraShakeTime = 1.0f, float cameraShakeStrength = 0.2f, DirectX::XMFLOAT2 cameraShakeOffset = { 0, 0.05f }, float cameraShakeSpeed = 50.0f)
	{
		if (!cameraShakeShader)
		{
			cameraShakeShader = true;                // �J�����V�F�C�N(�V�F�[�_�[)������
			cameraShakeShaderTime = cameraShakeTime;     // �J�����V�F�C�N(�V�F�[�_�[)�̎���
			postEffect.shakeStrength = cameraShakeStrength; // �J�����V�F�C�N(�V�F�[�_�[)�̋���
			postEffect.shakeOffset = cameraShakeOffset;   // �J�����V�F�C�N(�V�F�[�_�[)�̂��炷�ʒu
			postEffect.shakeSpeed = cameraShakeSpeed;    // �J�����V�F�C�N(�V�F�[�_�[)�̑���
		}
	}

	/****************************************************************************************/
		/*! �Q�b�^�[ */

#if 1
	// �r���[�s��擾
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// �v���W�F�N�V�����s��擾
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// �J�����̈ʒu�擾
	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	// �����_�擾
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }
	// �J�����̏�����擾
	const DirectX::XMFLOAT3& GetUp()    const { return up; }
	// �J�����̐��ʕ����擾
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	// �J�����̉E�����擾
	const DirectX::XMFLOAT3& GetRight() const { return right; }

#endif

	/****************************************************************************************/

		// �|�X�g�G�t�F�N�g�̃��Z�b�g
	bool PostEffectStatusReset();

	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�ύX����
	void UpdatePostEffectStatusChange(float elapsedTime);

	// �R���g���X�g�ύX�X�V����
	bool UpdateContrastChange(float elapsedTime);

	// �T�`�����[�V����(�F�̍ʓx)�ύX�X�V����
	bool UpdateSaturationChange(float elapsedTime);

	// �J���[�t�B���^�[�ύX�X�V����
	bool UpdateColorFilterChange(float elapsedTime);

	// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�ύX�X�V����
	bool UpdateChromaticAberrationChange(float elapsedTime);

	// �u���[�̋��x�ύX�X�V����
	bool UpdateBlurStrengthChange(float elapsedTime);

	// �J�����V�F�C�N�X�V����
	bool UpdateCameraShakeShader(float elapsedTime);

	bool UpdateVignetteIntensityChange(float elapsedTime);

private:
	DirectX::XMFLOAT4X4 view;       // �r���[�s��
	DirectX::XMFLOAT4X4 projection; // �v���W�F�N�V�����s��

	DirectX::XMFLOAT3 eye;   // �J�����̈ʒu
	DirectX::XMFLOAT3 focus; // �J�����̒����_

	DirectX::XMFLOAT3 up;    // �J�����̏����
	DirectX::XMFLOAT3 front; // �J�����̐��ʕ���
	DirectX::XMFLOAT3 right; // �J�����̉E����

private:
	bool setPostEffectStatusOnce = false; //! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̐ݒ�(��񂾂�)��������

public:
	/*! ���Z�b�g */
#if 1
	static bool  postEffectReset;              //! �|�X�g�G�t�F�N�g�̃��Z�b�g
	static float contrastReset;                // �R���g���X�g�̃��Z�b�g���̐��l
	static float saturationReset;              // �T�`�����[�V�����̃��Z�b�g�̐��l
	static DirectX::XMFLOAT3 colorFilterReset; // �J���[�t�B���^�[�̃��Z�b�g�̐��l
	static float chromaticAberrationReset;     // �N���}�e�B�b�N�A�x���[�V�����̃��Z�b�g�̐��l
	static float hueShiftRest;                 // �F���V�t�g�̃��Z�b�g���l
	static float blurStrengthReset;            // �u���[�̋��x�̃��Z�b�g�̐��l
	static float bloomThresholdReset;          // �O���A��臒l�̃��Z�b�g�̐��l
	static float bloomIntensityReset;          // �O���A�̋��x�̃��Z�b�g�̐��l
	static float focusDistanceReset;           // �s���g�����킹�鋗���̃��Z�b�g�̐��l
	static float focusRangeReset;              // �s���g�������͈͂̃��Z�b�g�̐��l
	static float dofBlurStrengthRest;          // ��ʊE�[�x�̃u���[���x�̃��Z�b�g�̐��l
#endif

	static bool  contrastChange;            // �R���g���X�g��ς��邩
	static float startContrastChange;       // �R���g���X�g�̕ύX�̊J�n�̒l
	static float endContrastChange;         // �����܂ŃR���g���X�g��ς���
	static float contrastChangeTime;        // �R���g���X�g�̒l��ς��鎞��
	static float contrastChangeElapsedTime; // �R���g���X�g�ύX�̌o�ߎ���

	static bool  saturationChange;            // �T�`�����[�V������ς��邩
	static float startSaturationChange;       // �T�`�����[�V�����̕ύX�̊J�n�̒l
	static float endSaturationChange;         // �����܂ŃT�`�����[�V����(�F�̍ʓx)��ς���
	static float saturationChangeTime;        // �T�`�����[�V�����̒l��ς��鎞��
	static float saturationChangeElapsedTime; // �T�`�����[�V�����ύX�̌o�ߎ���

	static bool colorFilterChange;                   // �J���[�t�B���^�[��ς��邩
	static DirectX::XMFLOAT3 startColorFilterChange; // �J���[�t�B���^�[�̕ύX�̊J�n�̒l
	static DirectX::XMFLOAT3 endColorFilterChange;   // �����܂ŃJ���[�t�B���^�[(�F�t�B���^�[)�̒l��ς���
	static float colorFilterChangeTime;              // �J���[�t�B���^�[�̒l��ς��鎞��
	static float colorFilterChangeElapsedTime;       // �J���[�t�B���^�[�ύX�̌o�ߎ���

	static bool  chromaticAberrationChange;            // �N���}�e�B�b�N�A�x���[�V������ς��邩
	static float startChromaticAberrationChange;       // �N���}�e�B�b�N�A�x���[�V�����̕ύX�̊J�n�̒l
	static float endChromaticAberrationChange;         // �����܂ŃN���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�̒l��ς���
	static float chromaticAberrationChangeTime;        // �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鎞��
	static float chromaticAberrationChangeElapsedTime; // �N���}�e�B�b�N�A�x���[�V�����ύX�̌o�ߎ���

	static bool  blurStrengthChange;            // �u���[�̋��x��ς��邩
	static float startBlurStrengthChange;       // �u���[�̋��x�̕ύX�̊J�n�̒l
	static float endBlurStrengthChange;         // �����܂Ńu���[�̋��x��ς���
	static float blurStrengthChangeTime;        // �u���[�̋��x��ς��鎞��
	static float blurStrengthChangeElapsedTime; // �u���[�̋��x�ύX�̌o�ߎ���

	static bool  cameraShakeShader;     // �J�����V�F�C�N(�V�F�[�_�[)
	static float cameraShakeShaderTime; // �J�����V�F�C�N(�V�F�[�_�[)���鎞��

	static bool vignetteIntensityChange;
	static float startVignetteIntensityChange;
	static float endVignetteIntensityChange;
	static float vignetteIntensityChangeTime;
	static float vignetteIntensityChangeElapsedTime;

	//static bool vignetteOpacityChange;

};