#include "CollisionAttack.h"
#include "CollisionAttackManager.h"
#include "Game/Stage/StageManager.h"
#include "Graphics/Graphics.h"
#include "Other/Easing.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
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
#ifdef _DEBUG
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

//! �v���C���[3�Ƃ̐ڐG����
bool CollisionAttack::CollisionVsPlayer3()
{
	Player3_SoratobuHusenWari& player = Player3_SoratobuHusenWari::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectBoxVsCylinder(
		collisionPos,
		width,
		height,
		depth,
		player.GetCollisionPos(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		return true;
	}

	return false;
}