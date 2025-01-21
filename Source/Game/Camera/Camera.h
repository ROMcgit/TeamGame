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

	//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
	void SetPostEffectStatus(float contrast = 1.3f,
		float saturation = 0.7f,
		const DirectX::XMFLOAT3 ColorFilter = { 0.9f, 1.0f, 1.05f },
		float chromaticAberration = 0.01f);

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

	/****************************************************************************************/

		// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�ύX����
	bool UpdatePostEffectStatusChange(float elapsedTime);

	// �J���[�t�B���^�[�̕ύX����
	float UpdateColorComponent(float colorFilter, float speed, float colorFilterChangeUp, float toColorFilter, float elapsedTime);

	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����ɖ߂�
	bool UpdatepostEffectReset();

private:
	DirectX::XMFLOAT4X4 view;       // �r���[�s��
	DirectX::XMFLOAT4X4 projection; // �v���W�F�N�V�����s��

	DirectX::XMFLOAT3 eye;   // �J�����̈ʒu
	DirectX::XMFLOAT3 focus; // �J�����̒����_

	DirectX::XMFLOAT3 up;    // �J�����̏����
	DirectX::XMFLOAT3 front; // �J�����̐��ʕ���
	DirectX::XMFLOAT3 right; // �J�����̉E����

private:
	bool postEffectInitialization = false; //! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̏�����

public:
	static bool postEffectControll;        //! �Q�[�����ɐ��l���������悤�ɂ���(ImGui�p)
	static bool postEffectReset;           //! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����ɖ߂�

	static bool postEffectChange; // �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ύX���邩

	static float toContrastChange;    // �����܂ŃR���X�g���N�g��ς���
	static float contrastChangeSpeed; // �R���X�g���N�g�̒l��ς��鑬�x
	static bool  contrastUp;          // �R���X�g���N�g�̒l���㏸�����邩

	static float toSaturationChange;    // �����܂ŃT�`�����[�V����(�F�̍ʓx)��ς���
	static float saturationChangeSpeed; // �T�`�����[�V�����̒l��ς��鑬�x
	static bool  saturationUp;          // �T�`�����[�V�����l���㏸�����邩

	static DirectX::XMFLOAT3 toColorFilterChange;    // �����܂ŃJ���[�t�B���^�[(�F�t�B���^�[)�̒l��ς���
	static DirectX::XMFLOAT3 colorFilterChangeSpeed; // �J���[�t�B���^�[�̒l��ς��鑬�x
	struct ColorFilterUp
	{
		bool x = false;
		bool y = false;
		bool z = false;
	};
	static ColorFilterUp colorFilterUp;   // �J���[�t�B���^�[�̒l���㏸�����邩

	static float toChromaticAberrationChange;    // �����܂ŃN���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�̒l��ς���
	static float chromaticAberrationChangeSpeed; // �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鑬�x
	static bool  chromaticAberrationUp;          // �N���}�e�B�b�N�A�x���[�V�����̒l���㏸�����邩
};