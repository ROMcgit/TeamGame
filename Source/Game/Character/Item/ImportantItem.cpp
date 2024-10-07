#include "ImportantItem.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Item/ImportantItemManager.h"

// デバッグプリミティブ描画
void ImportantItem::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ球を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// 破棄
void ImportantItem::Destroy()
{
	ImportantItemManager::Instance().Remove(this);
}
