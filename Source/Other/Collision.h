#pragma once

#include <DirectXMath.h>
#include "Graphics/Model.h"

// �q�b�g����
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 }; // ���C�ƃ|���S���̌���
	DirectX::XMFLOAT3 normal = { 0,0,0 }; // �Փ˂����|���S���̖@���x�N�g��
	float distance = 0.0f;      // ���C�̎n�_�����_�܂ł̋���
	int materialIndex = -1;        // �Փ˂����|���S���̃}�e���A���ԍ�
	DirectX::XMFLOAT3 rotation = { 0,0,0 }; // ��]��
};

// �R���W����
class Collision
{
public:
	// ���Ƌ��̌�������
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	// �~���Ɖ~���̌�������
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	// ���Ɖ~���̌�������
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	// ���ƃ{�b�N�X�̌�������
	static bool IntersectSphereVsBox(
		const DirectX::XMFLOAT3& spherePosition, float sphereRadius,
		const DirectX::XMFLOAT3& boxPosition, float boxWidth, float boxHeight, float boxDepth,
		DirectX::XMFLOAT3& outBoxPosition);

	// �{�b�N�X�Ɖ~���̌�������
	static bool IntersectBoxVsCylinder(
		const DirectX::XMFLOAT3& boxPosition,
		float boxWidth,
		float boxHeight,
		float boxDepth,
		const DirectX::XMFLOAT3& cylPosition,
		float cylRadius,
		float cylHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	// ���C�ƃ��f���̌�������
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result
	);
};