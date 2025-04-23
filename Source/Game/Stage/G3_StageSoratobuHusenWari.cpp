#include "Game/Stage/G3_StageSoratobuHusenWari.h"
#include <imgui.h>

// �R���X�g���N�^
G3_StageSoratobuHusenWari::G3_StageSoratobuHusenWari()
{
	// �X�e�[�W���f����ǂݍ���
	model = std::make_unique <Model>("Data/Model/Stage/3.SoratobuHusenWari/StageSoratobuHusenWari.mdl");

	position.y = 13.0f;

	scale.x = scale.z = 0.5f;
	scale.y = 0.2f;
}

G3_StageSoratobuHusenWari::~G3_StageSoratobuHusenWari()
{
}

// �X�V����
void G3_StageSoratobuHusenWari::Update(float elapsedTime)
{
	// �X�e�[�W�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime, Object::Stage);

	// �ʒu�X�V����
	UpdatePosition(elapsedTime);

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
		// ���C�̎n�_�ƏI�_�����_�̈ʒu���v�Z
		DirectX::XMVECTOR localRayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(localEndVec, localStartVec));
		DirectX::XMVECTOR localHitPointVec = DirectX::XMVectorAdd(localStartVec, DirectX::XMVectorScale(localRayDir, localHit.distance));

		// �O��̃��[�J����Ԃ��獡��̃��[���h��Ԃ֕ϊ�
		DirectX::XMMATRIX currentWorld = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMVECTOR worldHitPointVec = DirectX::XMVector3TransformCoord(localHitPointVec, currentWorld);

		// ��_�̈ʒu���擾
		DirectX::XMFLOAT3 worldHitPoint;
		DirectX::XMStoreFloat3(&worldHitPoint, worldHitPointVec);
		hit.distance = localHit.distance;
		hit.position = worldHitPoint; // ��_�̈ʒu���q�b�g���ʂɐݒ�

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

// �ʒu�X�V����
void G3_StageSoratobuHusenWari::UpdatePosition(float elapsedTime)
{
	position.z -= 5 * elapsedTime;

	if (position.z < -80.0f)
		position.z = 200.0f;
}
