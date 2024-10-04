#include "Installation.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Installation/InstallationManager.h"

// デバッグプリミティブ描画
void Installation::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ球を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// 破棄
void Installation::Destroy()
{
	InstallationManager::Instance().Remove(this);
}
