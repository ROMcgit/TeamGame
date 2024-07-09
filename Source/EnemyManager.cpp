#include "EnemyManager.h"
#include "Collision.h"

EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
	Clear();
}

// 更新処理
void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	for (Enemy* enemy : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(),
			enemies.end(), enemy);

		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		delete enemy;
	}

	removes.clear();

	// 敵同士の衝突処理
	//CollisionEnemyVsEnemies();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// デバッグプリミティブ描画
void EnemyManager::DrawDebugPrimitive()
{
	for (Enemy* enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
	
}

// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

// エネミー全削除
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		if (enemy != nullptr)
		{
			delete enemy;
		}
		enemies.clear();
	}
}

void EnemyManager::Remove(Enemy* enemy)
{
	// 破棄リストに追加
	removes.insert(enemy);
}

void EnemyManager::CollisionEnemyVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy1 = enemyManager.GetEnemy(i);

		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* enemy2 = enemyManager.GetEnemy(j);

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
