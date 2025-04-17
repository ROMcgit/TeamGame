#include "Game/Stage/G3_StageSoratobuHusenWari.h"
#include <imgui.h>

// �R���X�g���N�^
G3_StageSoratobuHusenWari::G3_StageSoratobuHusenWari()
{
	// �X�e�[�W���f����ǂݍ���
	model = std::make_unique <Model>("Data/Model/Stage/3.SoratobuHusenWari/StageSoratobuHusenWari.mdl");

	scale.x = scale.y = scale.z = 0.5f;

}

G3_StageSoratobuHusenWari::~G3_StageSoratobuHusenWari()
{
}

// �X�V����
void G3_StageSoratobuHusenWari::Update(float elapsedTime)
{
	// �X�e�[�W�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	//���C�L���X�g�悤�Ƀ��f����ԍs��ɂ��邽�߂ɒP�ʍs���n��
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// �`�揈��
void G3_StageSoratobuHusenWari::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// �\���p�̂��߃��[���h�s����X�V����
	model->UpdateTransform(transform);

	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
	shader->Draw(dc, model.get());
}

// ���C�L���X�g
bool G3_StageSoratobuHusenWari::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return false;
}

// �f�o�b�OGUI
void G3_StageSoratobuHusenWari::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"�X�e�[�W���ԕ��D����"))
	{
		// �ʒu
		ImGui::DragFloat3(u8"�ʒu", &position.x);

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
