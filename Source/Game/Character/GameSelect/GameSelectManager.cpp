#include "Game/Character/GameSelect/GameSelectManager.h"
#include "Other/Collision.h"

GameSelectManager::GameSelectManager()
{
}

GameSelectManager::~GameSelectManager()
{
	Clear();
}

// 更新処理
void GameSelectManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//for (GameSelect* enemy : removes)
	//{
	//	// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
	//	std::vector<GameSelect*>::iterator it = std::find(enemies.begin(),
	//		enemies.end(), enemy);

	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	delete enemy;
	//}

	for (GameSelect* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<GameSelect>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();
}

// 描画処理
void GameSelectManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void GameSelectManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// デバッグプリミティブ描画
void GameSelectManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// エネミー登録
void GameSelectManager::Register(std::unique_ptr<GameSelect> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// エネミー全削除
void GameSelectManager::Clear()
{
	enemies.clear();
}

void GameSelectManager::Remove(GameSelect* enemy)
{
	// 破棄リストに追加
	removes.push_back(enemy);
}

// デバッグ用GUI描画
void GameSelectManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}