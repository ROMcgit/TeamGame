#include "Game/Stage/G4_StageOssanTataki.h"
#include <imgui.h>

// �R���X�g���N�^
G4_StageOssanTataki::G4_StageOssanTataki()
{
	// �X�e�[�W���f����ǂݍ���
	model = std::make_unique <Model>("Data/Model/Stage/4.OssanTataki/StageOssanTataki.mdl");

	scale.x = scale.z = 0.5f;
	scale.y = 0.2f;
}

G4_StageOssanTataki::~G4_StageOssanTataki()
{
}

// �X�V����
void G4_StageOssanTataki::Update(float elapsedTime)
{
	// �X�e�[�W�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	//���C�L���X�g�悤�Ƀ��f����ԍs��ɂ��邽�߂ɒP�ʍs���n��
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// �`�揈��
void G4_StageOssanTataki::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// �\���p�̂��߃��[���h�s����X�V����
	model->UpdateTransform(transform);

	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
	shader->Draw(dc, model.get());
}

// ���C�L���X�g
bool G4_StageOssanTataki::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return false;
}

// �f�o�b�OGUI
void G4_StageOssanTataki::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"�X�e�[�W��������@��"))
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
