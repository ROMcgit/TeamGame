#include "Camera.h"
#include "Graphics/ShaderSetting.h"
#include "Graphics/Graphics.h"
#include "Other/Easing.h"
#include <imgui.h>

//! �|�X�g�G�t�F�N�g
Camera::CBPostEffectParam Camera::postEffect = {};
Microsoft::WRL::ComPtr<ID3D11Buffer> Camera::CBPostEffect;

//----------------------------------------------------------//

//! �|�X�g�G�t�F�N�g�̐��l���������悤�ɂ���
bool Camera::postEffectControll = false;
//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����ɖ߂�
bool Camera::postEffectReset = false;

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
float Camera::contrastChangeCurrentTime = 0.0f;

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
float Camera::saturationChangeCurrentTime = 0.0f;

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
float Camera::colorFilterChangeCurrentTime = 0.0f;

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
float Camera::chromaticAberrationChangeCurrentTime = 0.0f;

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
void Camera::SetPostEffectStatusOnce(float contrast, float saturation, const DirectX::XMFLOAT3 colorFilter, float chromaticAberration)
{
	// �p�����[�^������
	if (!setPostEffectStatusOnce)
	{
		//! �ύX����������
		contrastChange = false; // �R���g���X�g�̕ύX
		saturationChange = false; // �T�`�����[�V�����̕ύX
		colorFilterChange = false; // �J���[�t�B���^�[�̕ύX
		chromaticAberrationChange = false; // �N���}�e�B�b�N�A�x���[�V�����̕ύX

		//! �|�X�g�G�t�F�N�g
		postEffect.contrast = contrast;            // �R���g���X�g
		postEffect.saturation = saturation;          // �T�`�����[�V����
		postEffect.colorFilter = colorFilter;         // �J���[�t�B���^�[
		postEffect.chromaticAberration = chromaticAberration; // �N���}�e�B�b�N�A�x���[�V����

		setPostEffectStatusOnce = true; // �|�X�g�G�t�F�N�g�̃X�e�[�^�X�̐ݒ�(��񂾂�)������
	}
}

// �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
void Camera::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3 colorFilter, float chromaticAberration)
{
	//! �p�����[�^�̐ݒ�
	if (!postEffectControll)
	{
		postEffect.contrast = contrast;            // �R���g���X�g
		postEffect.saturation = saturation;          // �T�`�����[�V����
		postEffect.colorFilter = colorFilter;         // �J���[�t�B���^�[
		postEffect.chromaticAberration = chromaticAberration; // �N���}�e�B�b�N�A�x���[�V����
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

//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�ύX����
void Camera::UpdatePostEffectStatusChange(float elapsedTime)
{
	//! �R���g���X�g�ύX�X�V����
	UpdateContrastChange(elapsedTime);

	//! �T�`�����[�V�����ύX�X�V����
	UpdateSaturationChange(elapsedTime);

	//! �J���[�t�B���^�[�ύX�X�V����
	UpdateColorFilterChange(elapsedTime);

	//! �N���}�e�B�b�N�A�x���[�V�����ύX�X�V����
	UpdateChromaticAberrationChange(elapsedTime);
}

//! �R���g���X�g�ύX�X�V����
bool Camera::UpdateContrastChange(float elapsedTime)
{
	//! �R���g���X�g��ς��Ȃ��Ȃ�A�������~�߂�
	if (!contrastChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	contrastChangeCurrentTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = contrastChangeCurrentTime / contrastChangeTime;

	//! �R���g���X�g��ύX����
	postEffect.contrast = Easing::Linear(startContrastChange, endContrastChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
		contrastChange = false;

	return true;
}

//! �T�`�����[�V����(�F�̍ʓx)�ύX�X�V����
bool Camera::UpdateSaturationChange(float elapsedTime)
{
	//! �T�`�����[�V������ς��Ȃ��Ȃ�A�������~�߂�
	if (!saturationChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	saturationChangeCurrentTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = saturationChangeCurrentTime / saturationChangeTime;

	//! �T�`�����[�V������ύX����
	postEffect.saturation = Easing::Linear(startSaturationChange, endSaturationChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
		saturationChange = false;

	return true;
}

//! �J���[�t�B���^�[�ύX�X�V����
bool Camera::UpdateColorFilterChange(float elapsedTime)
{
	//! �J���[�t�B���^�[��ς��Ȃ��Ȃ�A�������~�߂�
	if (!colorFilterChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	colorFilterChangeCurrentTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = colorFilterChangeCurrentTime / colorFilterChangeTime;

	//! �R���g���X�g��ύX����
	postEffect.colorFilter.x = Easing::Linear(startColorFilterChange.x, endColorFilterChange.x, t);
	postEffect.colorFilter.y = Easing::Linear(startColorFilterChange.y, endColorFilterChange.y, t);
	postEffect.colorFilter.z = Easing::Linear(startColorFilterChange.z, endColorFilterChange.z, t);

	//! �������~�߂�
	if (t >= 1.0f)
		colorFilterChange = false;

	return true;
}

//! �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))�ύX�X�V����
bool Camera::UpdateChromaticAberrationChange(float elapsedTime)
{
	//! �R���g���X�g��ς��Ȃ��Ȃ�A�������~�߂�
	if (!chromaticAberrationChange)
		return false;


	//! �o�ߎ��Ԃ��v��
	chromaticAberrationChangeCurrentTime += elapsedTime;

	//! �C�[�W���O�^�C��
	float t = chromaticAberrationChangeCurrentTime / chromaticAberrationChangeTime;

	//! �R���g���X�g��ύX����
	postEffect.chromaticAberration = Easing::Linear(startChromaticAberrationChange, endChromaticAberrationChange, t);

	//! �������~�߂�
	if (t >= 1.0f)
		chromaticAberrationChange = false;

	return true;
}