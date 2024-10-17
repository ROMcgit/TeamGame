#include "Game/Character/Yazirusi/YazirusiManager.h"

// コンストラクタ
YazirusiManager::YazirusiManager()
{
}

// デストラクタ
YazirusiManager::~YazirusiManager()
{
	Clear();
}

// 更新処理
void YazirusiManager::Update(float elapsedTime)
{
	// 更新処理
	for (Yazirusi* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}

	// 破棄処理
	// ※projectilesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Yazirusi* projectile : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Yazirusi*>::iterator it = std::find(projectiles.begin(),
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
void YazirusiManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Yazirusi* projectile : projectiles)
	{
		projectile->Render(context, shader);
	}
}

void YazirusiManager::DrawDebugPrimitive()
{
	for (Yazirusi* projectile : projectiles)
	{
		projectile->DrawDebugPrimitive();
	}
}

void YazirusiManager::Register(Yazirusi* projectile)
{
	projectiles.emplace_back(projectile);
}

void YazirusiManager::Clear()
{
	for (Yazirusi* projectile : projectiles)
	{
		if (projectile != nullptr)
		{
			delete projectile;
			projectile = nullptr;
		}
	}
}

void YazirusiManager::Remove(Yazirusi* projectile)
{
	// 破棄リストに追加
	removes.insert(projectile);
}
