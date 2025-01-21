#include "Camera.h"
#include "Graphics/ShaderSetting.h"
#include "Graphics/Graphics.h"
#include <imgui.h>

//! �|�X�g�G�t�F�N�g
Camera::CBPostEffectParam Camera::postEffect = {};
Microsoft::WRL::ComPtr<ID3D11Buffer> Camera::CBPostEffect;

//----------------------------------------------------------//

//! �|�X�g�G�t�F�N�g�̐��l���������悤�ɂ���
bool Camera::postEffectControll = false;
//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����ɖ߂�
bool Camera::postEffectReset = false;

//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ύX���邩
bool Camera::postEffectChange = false;

//----------------------------------------------------------//

//! �����܂ŃR���X�g���N�g��ς���
float Camera::toContrastChange = 0.0f;
//! �R���X�g���N�g�̒l��ς��鑬�x
float Camera::contrastChangeSpeed = 0.0f;
//! �R���X�g���N�g�̒l���㏸�����邩
bool Camera::contrastUp = 0.0f;

//----------------------------------------------------------//

//! �����܂ŃT�`�����[�V����(�F�̍ʓx)��ς���
float Camera::toSaturationChange = 0.0f;
//! �T�`�����[�V�����̒l��ς��鑬�x
float Camera::saturationChangeSpeed = 0.0f;
//! �T�`�����[�V�������㏸�����邩
bool Camera::saturationUp = false;

//----------------------------------------------------------//

//! �����܂ŃJ���[�t�B���^�[(�F�t�B���^�[)��ς���
DirectX::XMFLOAT3 Camera::toColorFilterChange = { 0, 0, 0 };
//! �J���[�t�B���^�[�̒l��ς��鑬�x
DirectX::XMFLOAT3 Camera::colorFilterChangeSpeed = { 0, 0, 0 };
//! �J���[�t�B���^�[���㏸�����邩
Camera::ColorFilterUp Camera::colorFilterUp = { false, false, false };

//----------------------------------------------------------//

//! �����܂ŃN���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))��ς���
float Camera::toChromaticAberrationChange = 0.0f;
//! �N���}�e�B�b�N�A�x���[�V�����̒l��ς��鑬�x
float Camera::chromaticAberrationChangeSpeed = 0.0f;
//! �N���}�e�B�b�N�A�x���[�V�������㏸�����邩
bool Camera::chromaticAberrationUp = false;


// �|�X�g�G�t�F�N�g�𐶐�
void Camera::CreatePostEffect()
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	// posteffect�\���̂̓��e�ŃR���X�^���g�o�b�t�@�X�V
	dc->UpdateSubresource(CBPostEffect.Get(), 0, 0, &postEffect, 0, 0);

	// �s�N�Z���V�F�[�_�[Slot5�ɐݒ�
	dc->PSSetConstantBuffers(5, 1, CBPostEffect.GetAddressOf());
}

// �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ݒ�
void Camera::SetPostEffectStatus(float contrast, float saturation, const DirectX::XMFLOAT3 ColorFilter, float chromaticAberration)
{
	// �p�����[�^������
	if (!postEffectControll)
	{
		postEffect.contrast = contrast;            // �R���X�g���N�g
		postEffect.saturation = saturation;          // �T�`�����[�V����
		postEffect.colorFilter = ColorFilter;         // �J���[�t�B���^�[
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
	postEffect.contrast = 1.3f;
	postEffect.saturation = 0.7f;
	postEffect.colorFilter = { 0.9f, 1.0f, 1.05f };
	postEffect.chromaticAberration = 0.01f;
}

// �X�V����
void Camera::Update(float elapsedTime)
{
	// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����ɖ߂�
	UpdatepostEffectReset();

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

		if (ImGui::CollapsingHeader("PostEffect"))
		{
			//! �Q�[�����ɐ��l���������悤�ɂ���
			ImGui::Checkbox("PostEffectControll", &postEffectControll);
			//! ���l�����Z�b�g����
			ImGui::Checkbox("PostEffectReset", &postEffectReset);

			// �R���X�g���N�g(���l���傫���قǁA���邢�����͂�薾�邭�A�Â������͂��Â��Ȃ�)
			ImGui::DragFloat(u8"�R���X�g���N�g", &postEffect.contrast, 0.01f);
			// �T�`�����[�V����(�F�̍ʓx)
			ImGui::DragFloat(u8"�ʓx", &postEffect.saturation, 0.01f);
			// �J���[�t�B���^�[(�F�t�B���^�[)
			ImGui::DragFloat3(u8"�F�t�B���^�[", &postEffect.colorFilter.x, 0.01f);
			// �N���}�e�B�b�N�A�x���[�V����(�F����(�F�Y��))
			ImGui::DragFloat(u8"�F�Y��", &postEffect.chromaticAberration, 0.01f);
		}

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

// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�ύX����
bool Camera::UpdatePostEffectStatusChange(float elapsedTime)
{
	//! �|�X�g�G�t�F�N�g�̃X�e�[�^�X��ύX���Ȃ��Ȃ�A�������~�߂�
	if (!postEffectChange)
		return false;

	//! �R���X�g���N�g�̒l��ς���
	postEffect.contrast += (contrastUp ? 1 : -1) * (contrastChangeSpeed * elapsedTime);
	// �ڕW�l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((contrastUp && postEffect.contrast > toContrastChange) || (!contrastUp && postEffect.contrast < toContrastChange))
		postEffect.contrast = toContrastChange;

	//! �T�`�����[�V�����̒l��ς���
	postEffect.saturation += (saturationUp ? 1 : -1) * (contrastChangeSpeed * elapsedTime);
	// �ڕW�l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((saturationUp && postEffect.saturation > toSaturationChange) || (!saturationUp && postEffect.saturation < toSaturationChange))
		postEffect.saturation = toSaturationChange;

	//! �J���[�t�B���^�[�̒l��ς��鑬�x
	postEffect.colorFilter.x = UpdateColorComponent(postEffect.colorFilter.x, colorFilterChangeSpeed.x, colorFilterUp.x, toColorFilterChange.x, elapsedTime);
	postEffect.colorFilter.y = UpdateColorComponent(postEffect.colorFilter.y, colorFilterChangeSpeed.y, colorFilterUp.y, toColorFilterChange.y, elapsedTime);
	postEffect.colorFilter.z = UpdateColorComponent(postEffect.colorFilter.z, colorFilterChangeSpeed.z, colorFilterUp.z, toColorFilterChange.z, elapsedTime);

	//! �N���}�e�B�b�N�A�x���[�V�����̒l��ς���
	postEffect.chromaticAberration += (chromaticAberrationUp ? 1 : -1) * (chromaticAberrationChangeSpeed * elapsedTime);
	// �ڕW�l�𒴂����ꍇ�A�ڎw���l�ɂ���
	if ((chromaticAberrationUp && postEffect.chromaticAberration > toChromaticAberrationChange) || (!chromaticAberrationUp && postEffect.chromaticAberration < toChromaticAberrationChange))
		postEffect.chromaticAberration = toChromaticAberrationChange;

	//! �ڎw���l�ɂȂ����珈�����~�߂�
	if (postEffect.contrast == toContrastChange
		&& postEffect.saturation == toSaturationChange
		&& postEffect.colorFilter.x == toColorFilterChange.x
		&& postEffect.colorFilter.y == toColorFilterChange.y
		&& postEffect.colorFilter.z == toColorFilterChange.z
		&& postEffect.chromaticAberration == toChromaticAberrationChange)
		postEffectChange = false;

	return true;
}

// �J���[�t�B���^�[�̕ύX����
float Camera::UpdateColorComponent(float colorFilter, float speed, float colorFilterChangeUp, float toColorFilterChange, float elapsedTime)
{
	// �J���[�t�B���^�[��ύX
	colorFilter += (colorFilterChangeUp ? 1 : -1) * (speed * elapsedTime);

	// �ڎw���l�𒴂����ꍇ�A�ڕW�l�ɂ���
	if ((colorFilterChangeUp && colorFilter > toColorFilterChange) || (!colorFilterChangeUp && colorFilter < toColorFilterChange))
		colorFilter = toColorFilterChange;

	// �J���[�t�B���^�[��Ԃ�
	return colorFilter;
}

// �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����ɖ߂�
bool Camera::UpdatepostEffectReset()
{
	//! ���Z�b�g�����Ȃ珈�����~�߂�
	if (postEffectReset)
		return false;

	postEffect.contrast = 1.3f;                  // �R���X�g���N�g
	postEffect.saturation = 0.7f;                  // �T�`�����[�V����
	postEffect.colorFilter = { 0.9f, 1.0f, 1.05f }; // �J���[�t�B���^�[
	postEffect.chromaticAberration = 0.01f;                 // �N���}�e�B�b�N�A�x���[�V����

	postEffectReset = true; // �|�X�g�G�t�F�N�g�̃X�e�[�^�X�����Z�b�g����

	return true;
}
