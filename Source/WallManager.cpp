#include "WallManager.h"
#include "Collision.h"

WallManager::WallManager()
{
}

WallManager::~WallManager()
{
	Clear();
}

// 更新処理
void WallManager::Update(float elapsedTime)
{
	for (Wall* enemy : walls)
	{
		enemy->Update(elapsedTime);
	}

	for (Wall* enemy : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Wall*>::iterator it = std::find(walls.begin(),
			walls.end(), enemy);

		if (it != walls.end())
		{
			walls.erase(it);
		}

		delete enemy;
	}

	removes.clear();

	// 敵同士の衝突処理
	CollisionWallVsEnemies();
}

// 描画処理
void WallManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Wall* enemy : walls)
	{
		enemy->Render(context, shader);
	}
}

// デバッグプリミティブ描画
void WallManager::DrawDebugPrimitive()
{
	for (Wall* enemy : walls)
	{
		enemy->DrawDebugPrimitive();
	}

}

// エネミー登録
void WallManager::Register(Wall* enemy)
{
	walls.emplace_back(enemy);
}

// エネミー全削除
void WallManager::Clear()
{
	for (Wall* enemy : walls)
	{
		if (enemy != nullptr)
		{
			delete enemy;
		}
		walls.clear();
	}
}

void WallManager::Remove(Wall* enemy)
{
	// 破棄リストに追加
	removes.insert(enemy);
}

void WallManager::CollisionWallVsEnemies()
{
	WallManager& enemyManager = WallManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetWallCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Wall* enemy1 = enemyManager.GetWall(i);

		for (int j = i + 1; j < enemyCount; ++j)
		{
			Wall* enemy2 = enemyManager.GetWall(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			//if (Collision::IntersectSphereVsSphere(
			//	enemy1->GetPosition(),
			//	enemy1->GetRadius(),
			//	enemy2->GetPosition(),
			//	enemy2->GetRadius(),
			//	outPosition
			//))
			//{
			//	// 押し出しの後の位置設定
			//	enemy2->SetPosition(outPosition);
			//}

			if (Collision::IntersectCylinderVsCylinder(
				enemy1->GetPosition(),
				enemy1->GetRadius(),
				enemy1->GetHeight(),
				enemy2->GetPosition(),
				enemy2->GetRadius(),
				enemy2->GetHeight(),
				outPosition
			))
			{
				// 押し出しの後の位置設定
				enemy2->SetPosition(outPosition);
			}
		}
	}
}
