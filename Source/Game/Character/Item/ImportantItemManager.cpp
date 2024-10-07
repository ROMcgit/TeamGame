#include "Game/Character/Item/ImportantItemManager.h"
#include "Other/Collision.h"

ImportantItemManager::ImportantItemManager()
{
}

ImportantItemManager::~ImportantItemManager()
{
	Clear();
}

// 更新処理
void ImportantItemManager::Update(float elapsedTime)
{
	for (auto& item : importantItems)
	{
		item->Update(elapsedTime);
	}

	for (ImportantItem* item : removes)
	{
		auto it = std::remove_if(importantItems.begin(), importantItems.end(),
			[item](const std::unique_ptr<ImportantItem>& ptr) { return ptr.get() == item; });

		if (it != importantItems.end())
		{
			importantItems.erase(it, importantItems.end());
		}
	}

	removes.clear();
}

// 描画処理
void ImportantItemManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& item : importantItems)
	{
		item->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void ImportantItemManager::SpriteRender(ID3D11DeviceContext* dc)
{
	for (auto& item : importantItems)
	{
		item->SpriteRender(dc);
	}
}

// デバッグプリミティブ描画
void ImportantItemManager::DrawDebugPrimitive()
{
	for (auto& item : importantItems)
	{
		item->DrawDebugPrimitive();
	}
}

// エネミー登録
void ImportantItemManager::Register(std::unique_ptr<ImportantItem> item)
{
	importantItems.emplace_back(std::move(item));
}

// エネミー全削除
void ImportantItemManager::Clear()
{
	importantItems.clear();
}

void ImportantItemManager::Remove(ImportantItem* item)
{
	// 破棄リストに追加
	removes.push_back(item);
}

// デバッグ用GUI描画
void ImportantItemManager::DrawDebugGUI()
{
	for (auto& item : importantItems)
	{
		item->DrawDebugGUI();
	}
}