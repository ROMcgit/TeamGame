#pragma once

#include <vector>
#include"Wall.h"
#include <set>

// エネミーマネージャー
class WallManager
{
private:
	WallManager();
	~WallManager();

public:
	// 唯一のインスタンス取得
	static WallManager& Instance()
	{
		static WallManager instance;
		return instance;
	}

	// エネミー数取得
	int GetWallCount() const { return static_cast<int>(walls.size()); }

	// エネミー取得
	Wall* GetWall(int index) { return walls.at(index); }

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(Wall* enemy);

	// エネミー削除
	void Remove(Wall* enemy);

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// エネミー全削除
	void Clear();

private:
	// エネミー同士の衝突処理
	void CollisionWallVsEnemies();

	std::vector<Wall*> walls;
	std::set<Wall*> removes;
};