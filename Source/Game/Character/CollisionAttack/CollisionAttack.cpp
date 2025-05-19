#include "CollisionAttack.h"
#include "CollisionAttackManager.h"
#include "Game/Stage/StageManager.h"
#include "Graphics/Graphics.h"
#include "Other/Easing.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include <algorithm>

//! 衝突攻撃を止めるか(行列更新処理以外)
bool CollisionAttack::stopCollisionAttack = false;

CollisionAttack::CollisionAttack(CollisionAttackManager* manager)
	: manager(manager)
{
	debugPrimitiveColor = { 1, 0, 0 };           //! デバッグプリミティブの色(何も設定しない場合『赤』)
	collisionType       = CollisionType::Sphere; //! 当たり判定の種類(何も設定しない場合『球』)
}

// デバッグプリミティブ描画
void CollisionAttack::DrawDebugPrimitive()
{
#ifdef _DEBUG
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグを描画
	switch (collisionType)
	{
		//! ボックス
	case CollisionType::Box:
		debugRenderer->DrawBox(collisionPos, width, height, depth, DirectX::XMFLOAT4(debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1));
		break;
		//! 球
	case CollisionType::Sphere:
		debugRenderer->DrawSphere(collisionPos, radius, DirectX::XMFLOAT4(debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1));
		break;
		//! 円柱
	case CollisionType::Cylinder:
		debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1));
		break;
	default:
		break;
	}

#endif
}

// 破棄
void CollisionAttack::Destroy()
{
	manager->Remove(this);
}

//! プレイヤー3との接触処理
bool CollisionAttack::CollisionVsPlayer3()
{
	Player3_SoratobuHusenWari& player = Player3_SoratobuHusenWari::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectBoxVsCylinder(
		collisionPos,
		width,
		height,
		depth,
		player.GetCollisionPos(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		return true;
	}

	return false;
}