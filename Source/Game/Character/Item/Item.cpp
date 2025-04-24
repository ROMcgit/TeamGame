#include "Item.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Item/ItemManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"

// デバッグプリミティブ描画
void Item::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// 衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Item::Destroy()
{
	ItemManager::Instance().Remove(this);
}

// プレイヤー3との衝突処理
bool Item::CollisionVsPlayer3()
{
	Player3_SoratobuHusenWari& player = Player3_SoratobuHusenWari::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		collisionPos,
		radius,
		height,
		player.GetCollisionPos(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		return true;
	}

		return false;
}
