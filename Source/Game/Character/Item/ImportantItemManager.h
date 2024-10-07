#pragma once

#include <vector>
#include"ImportantItem.h"
#include <set>
#include <stdexcept>

// エネミーマネージャー
class ImportantItemManager
{
private:
	ImportantItemManager();
	~ImportantItemManager();

public:
	// 唯一のインスタンス取得
	static ImportantItemManager& Instance()
	{
		static ImportantItemManager instance;
		return instance;
	}

	// エネミー数取得
	int GetImportantItemCount() const { return static_cast<int>(importantItems.size()); }

	// エネミー取得
	std::unique_ptr<ImportantItem>& GetImportantItem(int index) {
		if (index < 0 || index >= static_cast<int>(importantItems.size()))
		{
			throw std::out_of_range("Item index is out of range");
		}
		return importantItems[index];
	}

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(std::unique_ptr<ImportantItem> enemy);

	// エネミー削除
	void Remove(ImportantItem* enemy);

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

	std::vector<std::unique_ptr<ImportantItem>> importantItems;
	std::vector<ImportantItem*> removes;
};