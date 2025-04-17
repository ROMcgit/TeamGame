#pragma once

#include <vector>
#include"Director.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// エネミーマネージャー
class DirectorManager
{
public:
	DirectorManager();
	~DirectorManager();

public:
	// 唯一のインスタンス取得
	static DirectorManager& Instance()
	{
		static DirectorManager instance;
		return instance;
	}

	// エネミー数取得
	int GetDirectorCount() const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	std::unique_ptr<Director>& GetDirector(int index) {
		if (index < 0 || index >= static_cast<int>(enemies.size()))
		{
			throw std::out_of_range("Director index is out of range");
		}
		return enemies[index];
	}

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(std::unique_ptr<Director> enemy);

	// エネミー削除
	void Remove(Director* enemy);

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

	std::vector<std::unique_ptr<Director>> enemies;
	std::vector<Director*> removes;
};