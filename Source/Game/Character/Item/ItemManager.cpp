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
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}


	for (Item* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<Item>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();
}

// 描画処理
void ItemManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void ItemManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		
	}
}

// デバッグプリミティブ描画
void ItemManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// エネミー登録
void ItemManager::Register(std::unique_ptr<Item> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// エネミー全削除
void ItemManager::Clear()
{
	enemies.clear();
}

void ItemManager::Remove(Item* enemy)
{
	// 破棄リストに追加
	removes.push_back(enemy);
}

// デバッグ用GUI描画
void ItemManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}