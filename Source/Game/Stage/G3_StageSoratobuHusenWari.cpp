#include "Game/Stage/G3_StageSoratobuHusenWari.h"

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
	// �s��X�V
	UpdateTransform();

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

// �s��X�V����
void G3_StageSoratobuHusenWari::UpdateTransform()
{
	// �ȑO�̕ϊ��s���ۑ�
	oldTransform = transform;

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
