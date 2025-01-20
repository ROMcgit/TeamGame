#pragma once

#include <vector>
#include"Obstacle.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// 障害物マネージャー
class ObstacleManager
{
private:
	ObstacleManager();
	~ObstacleManager();

public:
	// 唯一のインスタンス取得
	static ObstacleManager& Instance()
	{
		static ObstacleManager instance;
		return instance;
	}

	// エネミー数取得
	int GetObstacleCount() const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	std::unique_ptr<Obstacle>& GetObstacle(int index) {
		if (index < 0 || index >= static_cast<int>(enemies.size()))
		{
			throw std::out_of_range("Obstacle index is out of range");
		}
		return enemies[index];
	}

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(std::unique_ptr<Obstacle> enemy);

	// エネミー削除
	void Remove(Obstacle* enemy);

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

private:

	std::vector<std::unique_ptr<Obstacle>> enemies;
	std::vector<Obstacle*> removes;
};