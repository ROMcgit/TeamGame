#pragma once

#include <vector>
#include"GameSelect.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// エネミーマネージャー
class GameSelectManager
{
private:
	GameSelectManager();
	~GameSelectManager();

public:
	// 唯一のインスタンス取得
	static GameSelectManager& Instance()
	{
		static GameSelectManager instance;
		return instance;
	}

	// ゲームセレクト数取得
	int GetGameSelectCount() const { return static_cast<int>(gameSelects.size()); }

	// ゲームセレクト取得
	std::unique_ptr<GameSelect>& GetGameSelect(int index) {
		if (index < 0 || index >= static_cast<int>(gameSelects.size()))
		{
			throw std::out_of_range("GameSelect index is out of range");
		}
		return gameSelects[index];
	}

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(std::unique_ptr<GameSelect> enemy);

	// エネミー削除
	void Remove(GameSelect* enemy);

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

	std::vector<std::unique_ptr<GameSelect>> gameSelects;
	std::vector<GameSelect*> removes;
};