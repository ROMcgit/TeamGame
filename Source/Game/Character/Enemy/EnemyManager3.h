#pragma once

#include <vector>
#include"Enemy.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// エネミーマネージャー
class EnemyManager3
{
private:
	EnemyManager3();
	~EnemyManager3();

public:
	// 唯一のインスタンス取得
	static EnemyManager3& Instance()
	{
		static EnemyManager3 instance;
		return instance;
	}

	// エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	std::unique_ptr<Enemy>& GetEnemy(int index) {
		if (index < 0 || index >= static_cast<int>(enemies.size()))
		{
			throw std::out_of_range("Enemy index is out of range");
		}
		return enemies[index];
	}

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(std::unique_ptr<Enemy> enemy);

	// エネミー削除
	void Remove(Enemy* enemy);

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// 2Dスプライトの描画処理
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// エネミー全削除
	void Clear();

public:
	static bool enemyStop; // 敵を止めるか

public:

	std::vector<std::unique_ptr<Enemy>> enemies;
	std::vector<Enemy*> removes;
};