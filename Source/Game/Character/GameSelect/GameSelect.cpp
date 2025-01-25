#include "GameSelect.h"
#include "Graphics/Graphics.h"
#include "Game/Character/GameSelect/GameSelectManager.h"

// デバッグプリミティブ描画
void GameSelect::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ球を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void GameSelect::Destroy()
{
	GameSelectManager::Instance().Remove(this);
	//enemyManager->Remove(this);
}
