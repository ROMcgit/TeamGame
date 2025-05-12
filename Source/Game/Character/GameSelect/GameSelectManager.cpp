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
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->Update(elapsedTime);
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
		auto it = std::remove_if(gameSelects.begin(), gameSelects.end(),
			[enemy](const std::unique_ptr<GameSelect>& ptr) { return ptr.get() == enemy; });

		if (it != gameSelects.end())
		{
			gameSelects.erase(it, gameSelects.end());
		}
	}

	removes.clear();
}

// 描画処理
void GameSelectManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void GameSelectManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->SpriteRender(dc, graphics);
	}
}

// デバッグプリミティブ描画
void GameSelectManager::DrawDebugPrimitive()
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->DrawDebugPrimitive();
	}
}

// エネミー登録
void GameSelectManager::Register(std::unique_ptr<GameSelect> enemy)
{
	gameSelects.emplace_back(std::move(enemy));
}

// エネミー全削除
void GameSelectManager::Clear()
{
	gameSelects.clear();
}

void GameSelectManager::Remove(GameSelect* enemy)
{
	// 破棄リストに追加
	removes.push_back(enemy);
}

// デバッグ用GUI描画
void GameSelectManager::DrawDebugGUI()
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->DrawDebugGUI();
	}
}