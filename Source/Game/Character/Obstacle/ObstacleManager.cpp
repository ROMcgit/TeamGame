#include "Game/Character/Obstacle/ObstacleManager.h"
#include "Other/Collision.h"

ObstacleManager::ObstacleManager()
{
}

ObstacleManager::~ObstacleManager()
{
	Clear();
}

// 更新処理
void ObstacleManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//for (Obstacle* enemy : removes)
	//{
	//	// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
	//	std::vector<Obstacle*>::iterator it = std::find(enemies.begin(),
	//		enemies.end(), enemy);

	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	delete enemy;
	//}

	for (Obstacle* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<Obstacle>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();

	// 敵同士の衝突処理
	CollisionObstacleVsEnemies();
}

// 描画処理
void ObstacleManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void ObstacleManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// デバッグプリミティブ描画
void ObstacleManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// エネミー登録
void ObstacleManager::Register(std::unique_ptr<Obstacle> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// エネミー全削除
void ObstacleManager::Clear()
{
	enemies.clear();
}

void ObstacleManager::Remove(Obstacle* enemy)
{
	// 破棄リストに追加
	removes.push_back(enemy);
}

// デバッグ用GUI描画
void ObstacleManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}
