#pragma once

#include <vector>
#include"Enemy.h"
#include <set>

// エネミーマネージャー
class EnemyManager
{
private:
	EnemyManager();
	~EnemyManager();

public:
	// 唯一のインスタンス取得
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	// エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(Enemy* enemy);

	// エネミー削除
	void Remove(Enemy* enemy);

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// エネミー全削除
	void Clear();

private:
	// エネミー同士の衝突処理
	void CollisionEnemyVsEnemies();

	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};