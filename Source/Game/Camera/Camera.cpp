#include "Camera.h"
#include "Graphics/ShaderSetting.h"
#include "Graphics/Graphics.h"
#include "Game/Scene/Scene.h"
#include "Other/Easing.h"
#include <imgui.h>

//! �|�X�g�G�t�F�N�g
Camera::CBPostEffectParam Camera::postEffect = {};
Microsoft::WRL::ComPtr<ID3D11Buffer> Camera::CBPostEffect;

//----------------------------------------------------------//

//! �|�X�g�G�t�F�N�g�̐��l�����Z�b�g���邩
bool Camera::postEffectReset = false;
//! �R���g���X�g�̃��Z�b�g���̐��l
float Camera::contrastReset;
//! �T�`�����[�V�����̃��Z�b�g�̐��l
float Camera::saturationReset;
//! �J���[�t�B���^�[�̃��Z�b�g�̐��l
DirectX::XMFLOAT3 Camera::colorFilterReset;
//! �N���}�e�B�b�N�A�x���[�V�����̃��Z�b�g�̐��l
float Camera::chromaticAberrationReset;
//! �F���V�t�g�̃��Z�b�g���l
float Camera::hueShiftRest;
//! �u���[�̋��x�̃��Z�b�g�̐��l
float Camera::blurStrengthReset;
//! �O���A��臒l�̃��Z�b�g�̐��l
float Camera::bloomThresholdReset;
//! �O���A�̋��x�̃��Z�b�g�̐��l
float Camera::bloomIntensityReset;
//! �s���g�����킹�鋗���̃��Z�b�g�̐��l
float Camera::focusDistanceReset;
//! �s���g�������͈͂̃��Z�b�g�̐��l
float Camera::focusRangeReset;
//! ��ʊE�[�x�̃u���[���x�̃��Z�b�g�̐��l
float Camera::dofBlurStrengthRest;

//----------------------------------------------------------//

//! �R���g���X�g�̒l��ύX���邩
bool Camera::contrastChange = false;
//! �R���g���X�g�̕ύX�̊J�n�̒l
float Camera::startContrastChange = 0.0f;
//! �����܂ŃR���g���X�g��ς���
float Camera::endContrastChange = 0.0f;
//! �R���g���X�g�̒l��ς��鎞��
float Camera::contrastChangeTime = 0.0f;
//! �R���g���X�g�ύX�̌o�ߎ���
float Camera::contrastChangeElapsedTime = 0.0f;

//----------------------------------------------------------//

//! �T�`�����[�V�����̒l��ύX���邩
bool Camera::saturationChange = false;
//! �T�`�����[�V�����̕ύX�̊J�n�̒l
float Camera::startSaturationChange = 0.0f;
//! �����܂ŃT�`�����[�V����(�F�̍ʓx)��ς���
float Camera::endSaturationChange = 0.0f;
//! �T�`�����[�V�����̒l��ς��鎞��
float Camera::saturationChangeTime = 0.0f;
// !�T�`�����[�V�����ύX�̌o�ߎ���
float Camera::saturationChangeElapsedTime = 0.0f;

//----------------------------------------------------------//

//! �J���[�t�B���^�[�̒l��ύX���邩
bool Camera::colorFilterChange = false;
//! �J���[�t�B���^�[�̕ύX�̊J�n�̒l
DirectX::XMFLOAT3 Camera::startColorFilterChange = { 0, 0, 0 };
//! �����܂ŃJ���[�t�B���^�[(�F�t�B���^�[)��ς���
DirectX::XMFLOAT3 Camera::endColorFilterChange = { 0, 0, 0 };
//! �J���[�t�B���^�[�̒l��ς��鎞��
float Camera::colorFilterChangeTime = 0.0f;
//! �J���[�t�B���^�[�ύX�̌o�ߎ���
float Camera::colorFilterChangeElapsedTime = 0.0f;

//----------------------------------------------------------//

//! �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�̒l��ύX���邩
bool Camera::chromaticAberrationChange = false;
//! �N���}�e�B�b�N�A�x���[�V�����̕ύX�̊J�n�̒l
float Camera::startChromaticAberrationChange = 0.0f;
//! �����܂ŃN���}�e�B�b�N�A�x���[�V������ς���
float Camera::endChromaticAberrationChange = 0.0f;
//! �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鎞��
float Camera::chromaticAberrationChangeTime = 0.0f;
//! �N���}�e�B�b�N�A�x���[�V�����ύX�̌o�ߎ���
float Camera::chromaticAberrationChangeElapsedTime = 0.0f;

//----------------------------------------------------------//

//! �u���[�̋��x��ύX���邩
bool Camera::blurStrengthChange = false;
//! �u���[�̋��x�̕ύX�J�n�̒l
float Camera::startBlurStrengthChange = 0.0f;
//! �����܂Ńu���[�̋��x��ς���
float Camera::endBlurStrengthChange = 0.0f;
//! �u���[�̋��x��ς��鎞��
float Camera::blurStrengthChangeTime = 0.0f;
//! �u���[�̋��x�ύX�̌o�ߎ���
float Camera::blurStrengthChangeElapsedTime = 0.0f;

//----------------------------------------------------------//

//! �J�����V�F�C�N(�V�F�[�_�[)�����邩
bool Camera::cameraShakeShader = false;
//! �J�����V�F�C�N(�V�F�[�_�[)�����鎞��
float Camera::cameraShakeShaderTime = 0.0f;

// �|�X�g�G�t�F�N�g�𐶐�
void Camera::CreatePostEffect()
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	// posteffect�\���̂̓��e�ŃR���X�^���g�o�b�t�@�X�V
	dc->UpdateSubresource(CBPostEffect.Get(), 0, 0, &postEffect, 0, 0);

	// �s�N�Z���V�F�[�_�[Slot5�ɐݒ肷��
	dc->PSSetConstantBuffers(5, 1, CBPostEffect.GetAddressOf());
}

//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̐ݒ�(��񂾂�)
void Camera::SetPostEffectStatusOnce()
{
	// �p�����[�^������
	if (!setPostEffectStatusOnce)
	{
		//! �ύX����������
		contrastChange = false; // �R���g���X�g�̕ύX
		saturationChange = false; // �T�`�����[�V�����̕ύX
		colorFilterChange = false; // �J���[�t�B���^�[�̕ύX
		chromaticAberrationChange = false; // �N���}�e�B�b�N�A�x���[�V�����̕ύX
		blurStrengthChange = false; // �u���[�̋��x�̕ύX

		//! �|�X�g�G�t�F�N�g
		postEffect.contrast = contrastReset = Scene::contrastStatic;            // �R���g���X�g
		postEffect.saturation = saturationReset = Scene::saturationStatic;          // �T�`�����[�V����
		postEffect.colorFilter = colorFilterReset = Scene::colorFilterStatic;         // �J���[�t�B���^�[
		postEffect.chromaticAberration = chromaticAberrationReset = Scene::chromaticAberrationStatic; // �N���}�e�B�b�N�A�x���[�V����
		postEffect.hueShift = hueShiftRest = Scene::hueShiftStatic;            // �F���V�t�g 

		postEffect.vignetteIntensity = Scene::vignetteIntensityStatic;                    // �r�l�b�g�̋��x
		postEffect.vignetteOpacity = Scene::vignetteOpacityStatic;                      // �r�l�b�g�̕s�����x
		postEffect.blurStrength = blurStrengthReset = Scene::blurStrengthStatic;   // �u���[�̋��x
		postEffect.bloomThreshold = bloomThresholdReset = Scene::bloomThresholdStatic; // �O���A��臒l
		postEffect.bloomIntensity = bloomIntensityReset = Scene::bloomIntensityStatic; // �O���A�̋��x

		postEffect.focusDistance = focusDistanceReset = Scene::focusDistanceStatic;   // �s���g�����킹�鋗��
		postEffect.focusRange = focusRangeReset = Scene::focusRangeStatic;      // �s���g�������͈�
		postEffect.dofBlurStrength = dofBlurStrengthRest = Scene::dofBlurStrengthStatic; // ��ʊE�[�x�̃u���[���x

		postEffect.shakeOffset = { 0.0f, 0.0f }; // �J�����V�F�C�N(�V�F�[�_�[)�̂��炷�ʒu
		postEffect.shakeStrength = 0.0f;           // �J�����V�F�C�N(�V�F�[�_�[)�̋���
		postEffect.shakeTime = 0.0f;           // �J�����V�F�C�N(�V�F�[�_�[)�̎���
		postEffect.shakeSpeed = 0.0f;           // �J�����V�F�C�N(�V�F�[�_�[)�̑���

		setPostEffectStatusOnce = true; // �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̐ݒ�(��񂾂�)������
	}
}

// �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
void Camera::SetPostEffectStatus()
{
	//! �p�����[�^�̐ݒ�
	if (!postEffectReset)
	{
		postEffect.contrast = Scene::contrastStatic;            // �R���g���X�g
		postEffect.saturation = Scene::saturationStatic;          // �T�`�����[�V����
		postEffect.colorFilter = Scene::colorFilterStatic;         // �J���[�t�B���^�[
		postEffect.chromaticAberration = Scene::chromaticAberrationStatic; // �N���}�e�B�b�N�A�x���[�V����
		postEffect.hueShift = Scene::hueShiftStatic;            // �F���V�t�g

		postEffect.vignetteIntensity = Scene::vignetteIntensityStatic; // �r�l�b�g�̋��x
		postEffect.vignetteOpacity = Scene::vignetteOpacityStatic;   // �r�l�b�g�̕s�����x
		postEffect.blurStrength = Scene::blurStrengthStatic;      // �u���[�̋��x
		postEffect.bloomThreshold = Scene::bloomThresholdStatic;    // �O���A��臒l
		postEffect.bloomIntensity = Scene::bloomIntensityStatic;    // �O���A�̋��x

		postEffect.focusDistance = Scene::focusDistanceStatic;   // �s���g�����킹�鋗��
		postEffect.focusRange = Scene::focusRangeStatic;      // �s���g�������͈�
		postEffect.dofBlurStrength = Scene::dofBlurStrengthStatic; // ��ʊE�[�x�̃u���[���x
	}
}

// �R���X�g���N�^
Camera::Camera()
{
	// �|�X�g�G�t�F�N�g������
	ShaderSetting::CreateConstantBuffer(
		Graphics::Instance().GetDevice(),
		sizeof(CBPostEffectParam),
		CBPostEffect.GetAddressOf());

	// �p�����[�^������
	postEffect.contrast = 1.0f;
	postEffect.saturation = 0.8f;
	postEffect.colorFilter = { 1.2f, 1.3f, 1.35f };
	postEffect.chromaticAberration = 0.0f;
}

// �X�V����
void Camera::Update(float elapsedTime)
{
	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�ύX����
	UpdatePostEffectStatusChange(elapsedTime);
}

// �f�o�b�OGUI�`��
void Camera::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(0, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::TreeNode("Camera"))
	{
		ImGui::InputFloat3("Eye", &eye.x);
		ImGui::InputFloat3("Focus", &focus.x);
		ImGui::InputFloat3("Up", &up.x);

		ImGui::TreePop();
	}
}

// �w�����������
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	// ���_�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = { eye.x,eye.y,eye.z };
	DirectX::XMVECTOR Focus = { focus.x,focus.y,focus.z };
	DirectX::XMVECTOR Up = { up.x,up.y,up.z };
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	// �r���[���t�s�񉻂��A���[���h�s��ɖ߂�
	DirectX::CXMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	// �J�����̕��������o��
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	// ���_�A�����_��ۑ�
	this->eye = eye;
	this->focus = focus;
}

// �p�[�X�y�N�e�B�u�ݒ�
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	// ��p�A��ʔ䗦�A�N���b�v��������v���W�F�N�V�����s����쐬
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}

// �|�X�g�G�t�F�N�g�̃��Z�b�g
bool Camera::PostEffectStatusReset()
{
	//! �|�X�g�G�t�F�N�g�̃��Z�b�g�����Ȃ��Ȃ�A�������~�߂�
	if (!postEffectReset)
		return false;

	postEffect.contrast = contrastReset;            // �R���g���X�g
	postEffect.saturation = saturationReset;          // �T�`�����[�V����
	postEffect.colorFilter = colorFilterReset;         // �J���[�t�B���^�[
	postEffect.chromaticAberration = chromaticAberrationReset; // �N���}�e�B�b�N�A�x���[�V����
	postEffect.hueShift = hueShiftRest;             // �F���V�t�g

	postEffect.vignetteIntensity = 0.0f;                // �r�l�b�g�̋��x
	postEffect.vignetteOpacity = 0.0f;                // �r�l�b�g�̕s�����x
	postEffect.blurStrength = blurStrengthReset;   // �u���[�̋��x
	postEffect.bloomThreshold = bloomThresholdReset; // �O���A��臒l
	postEffect.bloomIntensity = bloomIntensityReset; // �O���A�̋��x

	postEffect.focusDistance = focusDistanceReset;  // �s���g�����킹�鋗��
	postEffect.focusRange = focusRangeReset;     // �s���g�������͈�
	postEffect.dofBlurStrength = dofBlurStrengthRest; // ��ʊE�[�x�̃u���[���x

	//! �|�X�g�G�t�F�N�g�̃��Z�b�g������
	postEffectReset = false;

	return true;
}

//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�ύX����
void Camera::UpdatePostEffectStatusChange(float elapsedTime)
{
	//! �|�X�g�G�t�F�N�g�̃��Z�b�g
	PostEffectStatusReset();

	//! �R���g���X�g�ύX�X�V����
	UpdateContrastChange(elapsedTime);

	//! �T�`�����[�V�����ύX�X�V����
	UpdateSaturationChange(elapsedTime);

	//! �J���[�t�B���^�[�ύX�X�V����
	UpdateColorFilterChange(elapsedTime);

	//! �N���}�e�B�b�N�A�x���[�V�����ύX�X�V����
	UpdateChromaticAberrationChange(elapsedTime);

	//! �u���[�̋��x�ύX�X�V����
	UpdateBlurStrengthChange(elapsedTime);

	//! �J�����V�F�C�N(�V�F�[�_�[)�X�V����
	UpdateCameraShakeShader(elapsedTime);
}

//! �R���g���X�g�ύX�X�V����
bool Camera::UpdateContrastChange(float elapsedTime)
{
	//! �R���g���X�g��ς��Ȃ��Ȃ�A�������~�߂�
	if (!contrastChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	contrastChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = contrastChangeElapsedTime / contrastChangeTime;

	//! �R���g���X�g��ύX����
	postEffect.contrast = Easing::Linear(startContrastChange, endContrastChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		postEffect.contrast = endContrastChange;
		contrastChange = false;
	}

	return true;
}

//! �T�`�����[�V����(�F�̍ʓx)�ύX�X�V����
bool Camera::UpdateSaturationChange(float elapsedTime)
{
	//! �T�`�����[�V������ς��Ȃ��Ȃ�A�������~�߂�
	if (!saturationChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	saturationChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = saturationChangeElapsedTime / saturationChangeTime;

	//! �T�`�����[�V������ύX����
	postEffect.saturation = Easing::Linear(startSaturationChange, endSaturationChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		postEffect.saturation = endSaturationChange;
		saturationChange = false;
	}

	return true;
}

//! �J���[�t�B���^�[�ύX�X�V����
bool Camera::UpdateColorFilterChange(float elapsedTime)
{
	//! �J���[�t�B���^�[��ς��Ȃ��Ȃ�A�������~�߂�
	if (!colorFilterChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	colorFilterChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = colorFilterChangeElapsedTime / colorFilterChangeTime;

	//! �J���[�t�B���^�[��ύX����
	postEffect.colorFilter.x = Easing::Linear(startColorFilterChange.x, endColorFilterChange.x, t);
	postEffect.colorFilter.y = Easing::Linear(startColorFilterChange.y, endColorFilterChange.y, t);
	postEffect.colorFilter.z = Easing::Linear(startColorFilterChange.z, endColorFilterChange.z, t);

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		postEffect.colorFilter = endColorFilterChange;
		colorFilterChange = false;
	}

	return true;
}

//! �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�ύX�X�V����
bool Camera::UpdateChromaticAberrationChange(float elapsedTime)
{
	//! �R���g���X�g��ς��Ȃ��Ȃ�A�������~�߂�
	if (!chromaticAberrationChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	chromaticAberrationChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = chromaticAberrationChangeElapsedTime / chromaticAberrationChangeTime;

	//! �N���}�e�B�b�N�A�x���[�V������ύX����
	postEffect.chromaticAberration = Easing::Linear(startChromaticAberrationChange, endChromaticAberrationChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		postEffect.chromaticAberration = endChromaticAberrationChange;
		chromaticAberrationChange = false;
	}

	return true;
}

//! �u���[�̋��x�ύX�X�V����
bool Camera::UpdateBlurStrengthChange(float elapsedTime)
{
	//! �u���[�̋��x��ς��Ȃ��Ȃ�A�������~�߂�
	if (!blurStrengthChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	blurStrengthChangeElapsedTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = blurStrengthChangeElapsedTime / blurStrengthChangeTime;

	//! �u���[�̋��x��ύX����
	postEffect.blurStrength = Easing::Linear(startBlurStrengthChange, endBlurStrengthChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
	{
		//! �l�̂���𖳂���
		postEffect.blurStrength = endBlurStrengthChange;
		chromaticAberrationChange = false;
	}

	return true;
}

// �J�����V�F�C�N(�V�F�[�_�[)�X�V����
bool Camera::UpdateCameraShakeShader(float elapsedTime)
{
	//! �J�����V�F�C�N(�V�F�[�_�[)�����Ȃ��Ȃ�A�������~�߂�
	if (!cameraShakeShader)
		return false;

	//! �J�����V�F�C�N(�V�F�[�_�[)�̎��Ԃ��v��
	postEffect.shakeTime += elapsedTime;

	//! �w�肵�����Ԃ𒴂�����A�������~�߂�
	if (postEffect.shakeTime > cameraShakeShaderTime)
	{
		postEffect.shakeTime = 0.0f;  // �J�����V�F�C�N�̌v�����Ԃ�0�ɂ���
		postEffect.shakeStrength = 0.0f;  // �J�����V�F�C�N�̋�����0�ɂ���
		cameraShakeShader = false; // �J�����V�F�C�N������
	}

	return true;
}
