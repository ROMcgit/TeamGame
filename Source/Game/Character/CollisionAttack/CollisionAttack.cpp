#include "CollisionAttack.h"
#include "CollisionAttackManager.h"
#include "Game/Stage/StageManager.h"
#include "Graphics/Graphics.h"
#include "Other/Easing.h"
#include <algorithm>

//! �ՓˍU�����~�߂邩(�s��X�V�����ȊO)
bool CollisionAttack::stopCollisionAttack = false;

CollisionAttack::CollisionAttack(CollisionAttackManager* manager)
	: manager(manager)
{
	debugPrimitiveColor = { 1, 0, 0 };           //! �f�o�b�O�v���~�e�B�u�̐F(�����ݒ肵�Ȃ��ꍇ�w�ԁx)
	collisionType       = CollisionType::Sphere; //! �����蔻��̎��(�����ݒ肵�Ȃ��ꍇ�w���x)
}

// �f�o�b�O�v���~�e�B�u�`��
void CollisionAttack::DrawDebugPrimitive()
{
#ifndef _DEBUG
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// �Փ˔���p�̃f�o�b�O��`��
	switch (collisionType)
	{
		//! �{�b�N�X
	case CollisionType::Box:
		debugRenderer->DrawBox(collisionPos, width, height, depth, DirectX::XMFLOAT4(debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1));
		break;
		//! ��
	case CollisionType::Sphere:
		debugRenderer->DrawSphere(collisionPos, radius, DirectX::XMFLOAT4(debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1));
		break;
		//! �~��
	case CollisionType::Cylinder:
		debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1));
		break;
	default:
		break;
	}

#endif
}

// �j��
void CollisionAttack::Destroy()
{
	manager->Remove(this);
}