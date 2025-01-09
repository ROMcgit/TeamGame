#include "Game/Character/Projectile/ProjectileManager.h"

// コンストラクタ
ProjectileManager::ProjectileManager()
{
}

// デストラクタ
ProjectileManager::~ProjectileManager()
{
	Clear();
}

// 更新処理
void ProjectileManager::Update(float elapsedTime)
{
	// 更新処理
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}

	// 破棄処理
	// ※projectilesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Projectile* projectile : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(),
			projectiles.end(), projectile);

		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		// 弾丸の破棄処理
		delete projectile;
	}

	removes.clear();
}

// 描画処理
void ProjectileManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(context, shader);
	}
}

void ProjectileManager::DrawDebugPrimitive()
{
	for (Projectile* projectile : projectiles)
	{
		projectile->DrawDebugPrimitive();
	}
}

void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		if (projectile != nullptr)
		{
			delete projectile;
			projectile = nullptr;
		}
	}
}

void ProjectileManager::Remove(Projectile* projectile)
{
	// 破棄リストに追加
	removes.insert(projectile);
}
