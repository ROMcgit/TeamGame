#include "Other/Collision.h"

// ���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A��B�̒P�ʃx�N�g�����Z�o
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// ��������
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}

	// A��B�������o��
	DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(Vec);
	DirectX::XMVECTOR pushOutVec = DirectX::XMVectorScale(direction, sqrtf(lengthSq) - range);
	DirectX::XMVECTOR newPositionB = DirectX::XMVectorAdd(PositionB, pushOutVec);
	DirectX::XMStoreFloat3(&outPositionB, newPositionB);

	return true;
}

bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA, 
	float radiusA,
	float heightA, 
	const DirectX::XMFLOAT3& positionB, 
	float radiusB, 
	float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}

	// A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}

	// XZ���ʂł͈̔̓`�F�b�N
	float dx = positionA.x - positionB.x;
	float dz = positionA.z - positionB.z;
	float distanceSquared = dx * dx + dz * dz; //���K��
	float radiusSum = radiusA + radiusB;

	if (distanceSquared > radiusSum * radiusSum)
	{
		return false;
	}

	//A��B�������o��
	float distance = sqrtf(distanceSquared);
	float overlap = radiusSum - distance;

	// ���K��
	float normX = dx / distance;
	float normZ = dz / distance;
	
	outPositionB.x = positionB.x - normX * overlap;
	outPositionB.z = positionB.z - normZ * overlap;
	// Y���W�͕ς��Ȃ�
	outPositionB.y = positionB.y;


	return true;
}

// ���Ɖ~���̌�������
bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition, 
	float sphereRadius, 
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight, 
	DirectX::XMFLOAT3& outCylinderPosition)
{
	// �~���̒�ʒ��S���狅�̒��S�ւ̃x�N�g��
	DirectX::XMFLOAT3 delta = {
		spherePosition.x - cylinderPosition.x,
		0.0f, // Y�����͖���
		spherePosition.z - cylinderPosition.z
	};

	// XZ���ʂł̋�����2��
	float distanceXZ2 = delta.x * delta.x + delta.z * delta.z;
	float radiusSum = sphereRadius + cylinderRadius;


	// XZ���ʂł̋������A���̔��a�Ɖ~���̔��a�̘a�ȉ��Ȃ����
	if (distanceXZ2 <= radiusSum * radiusSum) {
		// ���̒��S���~���̍����͈͓��ɂ��邩�m�F
		float sphereBottom = spherePosition.y - sphereRadius;
		float sphereTop = spherePosition.y + sphereRadius;
		float cylinderBottom = cylinderPosition.y;
		float cylinderTop = cylinderPosition.y + cylinderHeight;

		if (sphereTop >= cylinderBottom && sphereBottom <= cylinderTop) {
			// �������Ă���ꍇ�A�����ʒu���v�Z
			outCylinderPosition = cylinderPosition;
			return true;
		}
	}

	return false;
}

// ���C�ƃ��f���̌�������
bool Collision::IntersectRayVsModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const Model* model,
	HitResult& result)
{
#if 0
	// �ȑO�̏����������������悤�ɉ��̎���
	if (end.y < 0.0f)
	{
		result.position.x = end.x;
		result.position.y = 0.0f;
		result.position.z = end.z;
		result.normal.x = 0.0f;
		result.normal.y = 1.0f;
		result.normal.z = 0.0f;
		return true;
	}
	return false;
#endif

	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	// ���[���h��Ԃ̃��C�̒���
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resouce = model->GetResource();
	for (const ModelResource::Mesh& mesh : resouce->GetMeshes())
	{
		// ���b�V���m�[�h�擾
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

		// ���C�̒���
		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		// �O�p�`(��)�Ƃ̌�������
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				// �O�p�`�̒��_�𒊏o
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				// �O�p�`�̎O�Ӄx�N�g�����Z�o
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

				// �O�p�`�̖@���x�N�g�����Z�o
				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB,BC);

				// ���ς̌��ʂ��v���X�Ȃ痠����
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
				float d;
				DirectX::XMStoreFloat(&d, Dot);

				if (d >= 0.0f) continue;

				// ���C�ƕ��ʂ̌������Z�o
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N, SA), Dot);

				float x;
				DirectX::XMStoreFloat(&x, X);
				if (x < 0 || x > neart) continue;

				DirectX::XMVECTOR P = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(V, X), S);

				// ��_���O�p�`�̓����ɂ��邩����
				// 1��
				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);

				if (dot1 < 0) continue;
				// 2��
				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);

				if (dot2 < 0) continue;
				// 3��
				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);

				if (dot3 < 0) continue;

				// �ŒZ�������X�V
				// ��_�Ɩ@�����X�V
				HitPosition = P;
				HitNormal = N;
				materialIndex = subset.materialIndex;
			}
		}
		if (materialIndex >= 0)
		{
			// ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition,
				WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			// �q�b�g���ۑ�
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal,
					WorldTransform);
				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}
	return hit;
}