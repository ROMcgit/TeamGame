#include "Item.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Item/ItemManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"

// �f�o�b�O�v���~�e�B�u�`��
void Item::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// �Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Item::Destroy()
{
	ItemManager::Instance().Remove(this);
}

// �v���C���[3�Ƃ̏Փˏ���
bool Item::CollisionVsPlayer3()
{
	Player3_SoratobuHusenWari& player = Player3_SoratobuHusenWari::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		collisionPos,
		radius,
		height,
		player.GetCollisionPos(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		return true;
	}

		return false;
}
