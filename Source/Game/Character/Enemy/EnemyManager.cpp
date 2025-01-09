#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"

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
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//for (Enemy* enemy : removes)
	//{
	//	// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
	//	std::vector<Enemy*>::iterator it = std::find(enemies.begin(),
	//		enemies.end(), enemy);

	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	delete enemy;
	//}

	for (Enemy* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<Enemy>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();

	// 敵同士の衝突処理
	CollisionEnemyVsEnemies();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void EnemyManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// デバッグプリミティブ描画
void EnemyManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// エネミー登録
void EnemyManager::Register(std::unique_ptr<Enemy> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// エネミー全削除
void EnemyManager::Clear()
{
	enemies.clear();
}

void EnemyManager::Remove(Enemy* enemy)
{
	// 破棄リストに追加
	removes.push_back(enemy);
}

// デバッグ用GUI描画
void EnemyManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}

void EnemyManager::CollisionEnemyVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		std::unique_ptr<Enemy>& enemy1 = enemyManager.GetEnemy(i);

		for (int j = i + 1; j < enemyCount; ++j)
		{
			std::unique_ptr<Enemy>& enemy2 = enemyManager.GetEnemy(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			//if (Collision::IntersectSphereVsSphere(
			//	enemy1->GetPosition(),
			//	enemy1->GetRadius(),
			//	enemy2->GetPosition(),
			//	enemy2->GetRadius(),
			//	outPosition
			//)
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
