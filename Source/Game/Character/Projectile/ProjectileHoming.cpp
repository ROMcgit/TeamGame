#include "Game/Character/Projectile/ProjectileHoming.h"

// �R���X�g���N�^
ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
	: Projectile(manager)
{
	model = new Model("Data/Model/Sword/Sword.mdl");

	// ���f�����������̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 6.0f;
}

// �f�X�g���N�^
ProjectileHoming::~ProjectileHoming()
{
	delete model;
}

// �X�V����
void ProjectileHoming::Update(float elapsedTime)
{
	// ��������
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0)
	{
		// �������폜
		Destroy();
	}

	// �ړ�
	{
		float moveSpeed = this->moveSpeed / 2 * elapsedTime;
		DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR directionVec = DirectX::XMLoadFloat3(&direction);
		positionVec = DirectX::XMVectorAdd(positionVec, DirectX::XMVectorScale(directionVec, moveSpeed));
		DirectX::XMStoreFloat3(&position, positionVec);
		position.x += direction.x * moveSpeed;
		position.y += direction.y * moveSpeed;
		position.z += direction.z * moveSpeed;
	}

	// ����
	{
		float turnSpeed = this->turnSpeed / 2 * elapsedTime;

		// �^�[�Q�b�g�܂ł̃x�N�g�����Z�o
		DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
		DirectX::XMVECTOR vecToTarget = DirectX::XMVectorSubtract(targetVec, positionVec);

		// �[���x�N�g���łȂ��Ȃ��]����
		DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(vecToTarget);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, LengthSq);
		if (lengthSq > 0.00001f)
		{
			// �^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
			vecToTarget = DirectX::XMVector3Normalize(vecToTarget);

			// �����Ă�������x�N�g�����Z�o
			DirectX::XMVECTOR directionVec = DirectX::XMLoadFloat3(&direction);

			// �O�������x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓���(�p�x)���Z�o
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(directionVec, vecToTarget);

			float dot;
			DirectX::XMStoreFloat(&dot, Dot);

			// �Q�̒P�ʃx�N�g���̊p�x���������قǁA
			// 1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����

			float adjustedTurnSpeed = 1.0f - dot; // ���ς�1.0�ɋ߂��قǉ�]���x������������
			if (adjustedTurnSpeed > turnSpeed) adjustedTurnSpeed = turnSpeed;

			// ��]�s���P�ʍs��Ƀ��Z�b�g
			DirectX::XMMATRIX Rotation = DirectX::XMMatrixIdentity();

			// ��]�p�x������Ȃ��]����������
			if (dot < 0.99999f)
			{
				// ��]�����Z�o
				DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(directionVec, vecToTarget);
				Axis = DirectX::XMVector3Normalize(Axis);

				// ��]���Ɖ�]�ʂ����]�s����Z�o
				DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, adjustedTurnSpeed);

				// ���݂̍s�����]������
				DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
				Transform = DirectX::XMMatrixMultiply(Transform, Rotation);
				DirectX::XMStoreFloat4x4(&transform, Transform);

				// ��]��̑O�����������o���A�P�ʃx�N�g��������
				directionVec = DirectX::XMVector3TransformNormal(directionVec, Rotation);
				directionVec = DirectX::XMVector3Normalize(directionVec);
				DirectX::XMStoreFloat3(&direction, directionVec);
			}
		}
	}

	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

// ����
void ProjectileHoming::Launch(
	const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& position, 
	const DirectX::XMFLOAT3& target)
{
	{
		this->direction = direction;
		this->position = position;
		this->target = target;

		UpdateTransform();
	}
}