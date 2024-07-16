#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	DirectX::XMFLOAT3 offsetPosition = {
	position.x + offset.x,
	position.y + offset.y,
	position.z + offset.z
	};

	//// 衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	//debugRenderer->DrawCylinder(offsetPosition, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
	//enemyManager->Remove(this);
}
