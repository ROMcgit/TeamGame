#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"

// �f�o�b�O�v���~�e�B�u�`��
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// �Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
	//enemyManager->Remove(this);
}
