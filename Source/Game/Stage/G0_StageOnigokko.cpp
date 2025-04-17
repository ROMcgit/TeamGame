#include "Game/Stage/G0_StageOnigokko.h"
#include <imgui.h>

// �R���X�g���N�^
G0_StageOnigokko::G0_StageOnigokko()
{
	// �X�e�[�W���f����ǂݍ���
	model = std::make_unique <Model>("Data/Model/Stage/0.StageOnigokko/StageOnigokko.mdl");

	scale.x = scale.z = 0.5f;
	scale.y = 0.1f;

}

G0_StageOnigokko::~G0_StageOnigokko()
{
}

// �X�V����
void G0_StageOnigokko::Update(float elapsedTime)
{
	// �s��X�V
	UpdateTransform();

	//���C�L���X�g�悤�Ƀ��f����ԍs��ɂ��邽�߂ɒP�ʍs���n��
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// �`�揈��
void G0_StageOnigokko::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// �\���p�̂��߃��[���h�s����X�V����
	model->UpdateTransform(transform);

	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
	shader->Draw(dc, model.get());
}

// ���C�L���X�g
bool G0_StageOnigokko::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return false;
}

// �f�o�b�OGUI
void G0_StageOnigokko::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"�X�e�[�W���ɂ�����"))
	{
		// �ʒu
		ImGui::InputFloat3(u8"�ʒu", &position.x);

		// ��]
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::DragFloat3("�p�x", &a.x, 0.01f);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);

		// �X�P�[��
		ImGui::DragFloat3("�傫��", &scale.x, 0.01f);

		ImGui::TreePop();
	}
}

// �s��X�V����
void G0_StageOnigokko::UpdateTransform()
{
	// �ȑO�̕ϊ��s���ۑ�
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
