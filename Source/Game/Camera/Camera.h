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

	//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̐ݒ�(��񂾂�)
	void SetPostEffectStatusOnce(float contrast = 1.0f,
		float saturation = 0.8f,
		const DirectX::XMFLOAT3 colorFilter = { 1.2f, 1.3f, 1.35f },
		float chromaticAberration = 0);

	//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
	void SetPostEffectStatus(float contrast = 1.0f,
		float saturation = 0.8f,
		const DirectX::XMFLOAT3 colorFilter = { 1.2f, 1.3f, 1.35f },
		float chromaticAberration = 0);

public:
	//! �|�X�g�G�t�F�N�g�֘A
	struct CBPostEffectParam
	{
		float contrast;            // �R���X�g���N�g(���l���傫���قǁA���邢�����͂�薾�邭�A�Â������͂��Â��Ȃ�)
		float saturation;          // �T�`�����[�V����(�F�̍ʓx)
		float chromaticAberration; // �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))
		float dummy;  // 4��؂�p

		DirectX::XMFLOAT3 colorFilter; // �J���[�t�B���^�[(�F�t�B���^�[)
		float dummy2; // 4��؂�p
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
	static bool postEffectControll;        //! �Q�[�����ɐ��l���������悤�ɂ���(ImGui�p)
	static bool postEffectReset;           //! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����ɖ߂�

	static bool  contrastChange;            // �R���g���X�g��ς��邩
	static float startContrastChange;       // �R���g���X�g�̕ύX�̊J�n�̒l
	static float endContrastChange;         // �����܂ŃR���g���X�g��ς���
	static float contrastChangeTime;        // �R���g���X�g�̒l��ς��鎞��
	static float contrastChangeCurrentTime; // �R���g���X�g�ύX�̌o�ߎ���

	static bool  saturationChange;            // �T�`�����[�V������ς��邩
	static float startSaturationChange;       // �T�`�����[�V�����̕ύX�̊J�n�̒l
	static float endSaturationChange;         // �����܂ŃT�`�����[�V����(�F�̍ʓx)��ς���
	static float saturationChangeTime;        // �T�`�����[�V�����̒l��ς��鎞��
	static float saturationChangeCurrentTime; // �T�`�����[�V�����ύX�̌o�ߎ���

	static bool colorFilterChange;                   // �J���[�t�B���^�[��ς��邩
	static DirectX::XMFLOAT3 startColorFilterChange; // �J���[�t�B���^�[�̕ύX�̊J�n�̒l
	static DirectX::XMFLOAT3 endColorFilterChange;   // �����܂ŃJ���[�t�B���^�[(�F�t�B���^�[)�̒l��ς���
	static float colorFilterChangeTime;              // �J���[�t�B���^�[�̒l��ς��鎞��
	static float colorFilterChangeCurrentTime;       // �J���[�t�B���^�[�ύX�̌o�ߎ���

	static bool  chromaticAberrationChange;            // �N���}�e�B�b�N�A�x���[�V������ς��邩
	static float startChromaticAberrationChange;       // �N���}�e�B�b�N�A�x���[�V�����̕ύX�̊J�n�̒l
	static float endChromaticAberrationChange;         // �����܂ŃN���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�̒l��ς���
	static float chromaticAberrationChangeTime;        // �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鎞��
	static float chromaticAberrationChangeCurrentTime; // �N���}�e�B�b�N�A�x���[�V�����ύX�̌o�ߎ���
};