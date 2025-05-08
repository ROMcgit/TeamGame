#include "GameSelect.h"
#include "Graphics/Graphics.h"
#include "Game/Character/GameSelect/GameSelectManager.h"

// �f�o�b�O�v���~�e�B�u�`��
void GameSelect::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// �Փ˔���p�̃f�o�b�O����`��
	debugRenderer->DrawBox(collisionPos, width, height, depth, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void GameSelect::Destroy()
{
	GameSelectManager::Instance().Remove(this);
	//enemyManager->Remove(this);
}
