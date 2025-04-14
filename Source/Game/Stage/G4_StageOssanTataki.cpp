#include "Game/Stage/G4_StageOssanTataki.h"

// �R���X�g���N�^
G4_StageOssanTataki::G4_StageOssanTataki()
{
	// �X�e�[�W���f����ǂݍ���
	model = std::make_unique <Model>("Data/Model/Stage/4.OssanTataki/StageOssanTataki.mdl");

	scale.x = scale.y = scale.z = 0.5f;

}

G4_StageOssanTataki::~G4_StageOssanTataki()
{
}

// �X�V����
void G4_StageOssanTataki::Update(float elapsedTime)
{
	// �s��X�V
	UpdateTransform();

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

// �s��X�V����
void G4_StageOssanTataki::UpdateTransform()
{
	// �ȑO�̕ϊ��s���ۑ�
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
