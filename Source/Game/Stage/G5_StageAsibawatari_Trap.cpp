#include "Game/Stage/G5_StageAsibawatari_Trap.h"
#include <imgui.h>

// �R���X�g���N�^
G5_StageAsibawatari_Trap::G5_StageAsibawatari_Trap()
{
	// �X�e�[�W���f����ǂݍ���
	model = std::make_unique <Model>("Data/Model/Stage/5.Asibawatari/1.Trap/StageTrap.mdl");

	scale.x = scale.y = scale.z = 0.1f;

	opacity = 0.5f;
}

G5_StageAsibawatari_Trap::~G5_StageAsibawatari_Trap()
{
}

// �X�V����
void G5_StageAsibawatari_Trap::Update(float elapsedTime)
{
	noViewTime -= elapsedTime;

	// �X�e�[�W�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	//���C�L���X�g�悤�Ƀ��f����ԍs��ɂ��邽�߂ɒP�ʍs���n��
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);

	// �ړ��X�V����
	UpdateMove(elapsedTime);
}

// �`�揈��
void G5_StageAsibawatari_Trap::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// �\���p�̂��߃��[���h�s����X�V����
	model->UpdateTransform(transform);

	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
	if(noViewTime <= 0.0f)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// ���C�L���X�g
bool G5_StageAsibawatari_Trap::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return false;
}

// �f�o�b�OGUI
void G5_StageAsibawatari_Trap::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"�X�e�[�W�g���b�v"))
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

// �ړ�����
void G5_StageAsibawatari_Trap::UpdateMove(float elapsedTime)
{
	position.x -= moveSpeed * elapsedTime;

	if (position.x < -50.0f)
		Destroy();
}