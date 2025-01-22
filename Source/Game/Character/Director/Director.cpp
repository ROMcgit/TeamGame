#include "Director.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Director/DirectorManager.h"

// デバッグプリミティブ描画
void Director::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// 衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Director::Destroy()
{
	DirectorManager::Instance().Remove(this);
	//enemyManager->Remove(this);
}
