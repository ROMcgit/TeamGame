#include "Wall.h"
#include "Graphics/Graphics.h"
#include "WallManager.h"

// デバッグプリミティブ描画
void Wall::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// 衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Wall::Destroy()
{
	WallManager::Instance().Remove(this);
	//enemyManager->Remove(this);
}
