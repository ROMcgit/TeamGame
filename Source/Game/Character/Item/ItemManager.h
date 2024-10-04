#pragma once

#include <vector>
#include"Item.h"
#include <set>
#include <stdexcept>

// エネミーマネージャー
class ItemManager
{
private:
	ItemManager();
	~ItemManager();

public:
	// 唯一のインスタンス取得
	static ItemManager& Instance()
	{
		static ItemManager instance;
		return instance;
	}

	// エネミー数取得
	int GetItemCount() const { return static_cast<int>(items.size()); }

	// エネミー取得
	std::unique_ptr<Item>& GetItem(int index) {
		if (index < 0 || index >= static_cast<int>(items.size()))
		{
			throw std::out_of_range("Item index is out of range");
		}
		return items[index];
	}

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(std::unique_ptr<Item> enemy);

	// エネミー削除
	void Remove(Item* enemy);

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// 2Dスプライトの描画処理
	void SpriteRender(ID3D11DeviceContext* dc);

	// HP表示
	void RenderItemGauge(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	// エネミー全削除
	void Clear();

private:
	// エネミー同士の衝突処理
	void CollisionItemVsEnemies();

	std::vector<std::unique_ptr<Item>> items;
	std::vector<Item*> removes;
};