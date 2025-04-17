#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"

bool EnemyManager::enemyStop = false;

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
		if(!enemyStop)
		enemy->Update(elapsedTime);
	}

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