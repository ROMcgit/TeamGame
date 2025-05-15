#include "Game/Stage/G1_StageEndHole.h"
#include "Game/Character/Player/Player1_DarumasangaKoronda.h"
#include <imgui.h>

// �R���X�g���N�^
G1_StageEndHole::G1_StageEndHole()
{
	// �X�e�[�W���f����ǂݍ���
	model = std::make_unique <Model>("Data/Model/Stage/1.DarumasangaKoronda/StageEndHole.mdl");

	position.x = 0.0f;
	position.y = 110.0f;
	position.z = 54.0f;

	scale.x = scale.y = scale.z = 0.5f;
}

G1_StageEndHole::~G1_StageEndHole()
{
}

// �X�V����
void G1_StageEndHole::Update(float elapsedTime)
{
	// �X�e�[�W�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	//���C�L���X�g�悤�Ƀ��f����ԍs��ɂ��邽�߂ɒP�ʍs���n��
	const DirectX::XMFLOAT4X4 transformIdentity = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	model->UpdateTransform(transformIdentity);
}

// �`�揈��
void G1_StageEndHole::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// �\���p�̂��߃��[���h�s����X�V����
	model->UpdateTransform(transform);

	// �v���C���[�̃C���X�^���X�擾
	Player1_DarumasangaKoronda& player = Player1_DarumasangaKoronda::Instance();
	// ����
	dist = abs(player.GetPosition().z - position.z);

	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
	if (dist < 185)
		shader->Draw(dc, model.get());
}

// ���C�L���X�g
bool G1_StageEndHole::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	// �O��̃��[���h�s��Ƌt�s������߂�
	DirectX::XMMATRIX oldWorld = DirectX::XMLoadFloat4x4(&oldTransform);
	DirectX::XMMATRIX invOldWorld = DirectX::XMMatrixInverse(nullptr, oldWorld);

	// �O��̃��[�J����Ԃł̃��C�ɕϊ�
	DirectX::XMVECTOR rayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR rayEnd = DirectX::XMLoadFloat3(&end);

	DirectX::XMVECTOR localStartVec = DirectX::XMVector3TransformCoord(rayStart, invOldWorld);
	DirectX::XMVECTOR localEndVec = DirectX::XMVector3TransformCoord(rayEnd, invOldWorld);

	DirectX::XMFLOAT3 localStart, localEnd;
	DirectX::XMStoreFloat3(&localStart, localStartVec);
	DirectX::XMStoreFloat3(&localEnd, localEndVec);

	// ���[�J����Ԃł̃��C�Ƃ̌�_�����߂�
	HitResult localHit;
	if (Collision::IntersectRayVsModel(localStart, localEnd, model.get(), localHit))
	{
		// �O��̃��[�J����Ԃ��獡��̃��[���h��Ԃ֕ϊ�
		// �O�񂩂獡��̂����ĕύX���ꂽ���e������Ă���I�u�W�F�N�g�ɔ��f�����B
		// ���C�̎n�_�ƏI�_�����_��G1_StageEndHole���v�Z
		DirectX::XMVECTOR localRayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(localEndVec, localStartVec));
		DirectX::XMVECTOR localHitPointVec = DirectX::XMVectorAdd(localStartVec, DirectX::XMVectorScale(localRayDir, localHit.distance));

		// �O��̃��[�J����Ԃ��獡��̃��[���h��Ԃ֕ϊ�
		DirectX::XMMATRIX currentWorld = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMVECTOR worldHitPointVec = DirectX::XMVector3TransformCoord(localHitPointVec, currentWorld);

		// ��_��G1_StageEndHole���擾
		DirectX::XMFLOAT3 worldHitPoint;
		DirectX::XMStoreFloat3(&worldHitPoint, worldHitPointVec);
		hit.distance = localHit.distance;
		hit.position = worldHitPoint; // ��_��G1_StageEndHole���q�b�g���ʂɐݒ�

		// �@���̕ϊ�
		DirectX::XMVECTOR localNormalVec = DirectX::XMLoadFloat3(&localHit.normal);
		DirectX::XMVECTOR worldNormalVec = DirectX::XMVector3TransformNormal(localNormalVec, currentWorld);
		DirectX::XMStoreFloat3(&hit.normal, worldNormalVec);

		// ��]�������Z�o
		hit.rotation.x = angle.x;
		hit.rotation.y = angle.y;
		hit.rotation.z = angle.z;

		return true;
	}
	return false;
}

// �f�o�b�OGUI
void G1_StageEndHole::DrawDebugGUI()
{
	if (ImGui::TreeNode(u8"�X�e�[�W"))
	{
		ImGui::InputFloat(u8"����", &dist);

		// G1_StageEndHole
		ImGui::DragFloat3(u8"G1_StageEndHole", &position.x);

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