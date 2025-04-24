#include "Game/Character/Enemy/EnemyManager3.h"
#include "Other/Collision.h"

bool EnemyManager3::enemyStop = false;

EnemyManager3::EnemyManager3()
{
}

EnemyManager3::~EnemyManager3()
{
	Clear();
}

// 更新処理
void EnemyManager3::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		if (!enemyStop)
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
void EnemyManager3::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void EnemyManager3::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// デバッグプリミティブ描画
void EnemyManager3::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// エネミー登録
void EnemyManager3::Register(std::unique_ptr<Enemy> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// エネミー全削除
void EnemyManager3::Clear()
{
	enemies.clear();
}

void EnemyManager3::Remove(Enemy* enemy)
{
	// 破棄リストに追加
	removes.push_back(enemy);
}

// デバッグ用GUI描画
void EnemyManager3::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}