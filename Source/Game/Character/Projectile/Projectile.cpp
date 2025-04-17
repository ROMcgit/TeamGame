#include "Projectile.h"
#include "ProjectileManager.h"
#include "Graphics/Graphics.h"
#include "Other/Easing.h"
#include "Game/Stage/StageManager.h"
#include <algorithm>

Projectile::Projectile(ProjectileManager* manager)
	: manager(manager)
{
}

// デバッグプリミティブ描画
void Projectile::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG

	debugRenderer->DrawSphere(collisionPos, radius, DirectX::XMFLOAT4(0, 0, 1, 1));

#endif // DEBUG
}

// 破棄
void Projectile::Destroy()
{
	manager->Remove(this);
}