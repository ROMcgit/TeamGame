#include "Director.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Director/DirectorManager.h"

// �f�o�b�O�v���~�e�B�u�`��
void Director::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// �Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Director::Destroy()
{
	DirectorManager::Instance().Remove(this);
	//enemyManager->Remove(this);
}
