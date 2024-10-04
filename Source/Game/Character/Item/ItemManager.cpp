#include "Game/Character/Item/ItemManager.h"
#include "Other/Collision.h"

ItemManager::ItemManager()
{
}

ItemManager::~ItemManager()
{
	Clear();
}

// 更新処理
void ItemManager::Update(float elapsedTime)
{
	for (auto& item : items)
	{
		item->Update(elapsedTime);
	}

	for (Item* item : removes)
	{
		auto it = std::remove_if(items.begin(), items.end(),
			[item](const std::unique_ptr<Item>& ptr) { return ptr.get() == item; });

		if (it != items.end())
		{
			items.erase(it, items.end());
		}
	}

	removes.clear();
}

// 描画処理
void ItemManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& item : items)
	{
		item->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void ItemManager::SpriteRender(ID3D11DeviceContext* dc)
{
	for (auto& item : items)
	{
		item->SpriteRender(dc);
	}
}

// HP表示
void ItemManager::RenderItemGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	for (auto& item : items)
	{
		item->RenderItemGauge(dc, view, projection);
	}
}

// デバッグプリミティブ描画
void ItemManager::DrawDebugPrimitive()
{
	for (auto& item : items)
	{
		item->DrawDebugPrimitive();
	}
}

// エネミー登録
void ItemManager::Register(std::unique_ptr<Item> item)
{
	items.emplace_back(std::move(item));
}

// エネミー全削除
void ItemManager::Clear()
{
	items.clear();
}

void ItemManager::Remove(Item* item)
{
	// 破棄リストに追加
	removes.push_back(item);
}

// デバッグ用GUI描画
void ItemManager::DrawDebugGUI()
{
	for (auto& item : items)
	{
		item->DrawDebugGUI();
	}
}

void ItemManager::CollisionItemVsEnemies()
{
	ItemManager& itemManager = ItemManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int itemCount = itemManager.GetItemCount();
	for (int i = 0; i < itemCount; ++i)
	{
		std::unique_ptr<Item>& item1 = itemManager.GetItem(i);

		for (int j = i + 1; j < itemCount; ++j)
		{
			std::unique_ptr<Item>& item2 = itemManager.GetItem(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;

			if (Collision::IntersectCylinderVsCylinder(
				item1->GetPosition(),
				item1->GetRadius(),
				item1->GetHeight(),
				item2->GetPosition(),
				item2->GetRadius(),
				item2->GetHeight(),
				outPosition
			))
			{
				// 押し出しの後の位置設定
				item2->SetPosition(outPosition);
			}
		}
	}
}
