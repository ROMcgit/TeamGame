#include "WallManager.h"
#include "Collision.h"

WallManager::WallManager()
{
}

WallManager::~WallManager()
{
	Clear();
}

// 更新処理
void WallManager::Update(float elapsedTime)
{
	for (Wall* wall : walls)
	{
		wall->Update(elapsedTime);
	}

	for (Wall* wall : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Wall*>::iterator it = std::find(walls.begin(),
			walls.end(), wall);

		if (it != walls.end())
		{
			walls.erase(it);
		}

		delete wall;
	}

	removes.clear();

	// 敵同士の衝突処理
	//CollisionWallVsEnemies();
}

// 描画処理
void WallManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Wall* wall : walls)
	{
		wall->Render(context, shader);
	}
}

// デバッグプリミティブ描画
void WallManager::DrawDebugPrimitive()
{
	for (Wall* wall : walls)
	{
		wall->DrawDebugPrimitive();
	}

}

// エネミー登録
void WallManager::Register(Wall* wall)
{
	walls.emplace_back(wall);
}

// エネミー全削除
void WallManager::Clear()
{
	for (Wall* wall : walls)
	{
		if (wall != nullptr)
		{
			delete wall;
		}
		walls.clear();
	}
}

void WallManager::Remove(Wall* wall)
{
	// 破棄リストに追加
	removes.insert(wall);
}

void WallManager::CollisionWallVsEnemies()
{
	WallManager& wallManager = WallManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int wallCount = wallManager.GetWallCount();
	for (int i = 0; i < wallCount; ++i)
	{
		Wall* wall1 = wallManager.GetWall(i);

		for (int j = i + 1; j < wallCount; ++j)
		{
			Wall* wall2 = wallManager.GetWall(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			//if (Collision::IntersectSphereVsSphere(
			//	wall1->GetPosition(),
			//	wall1->GetRadius(),
			//	wall2->GetPosition(),
			//	wall2->GetRadius(),
			//	outPosition
			//))
			//{
			//	// 押し出しの後の位置設定
			//	wall2->SetPosition(outPosition);
			//}

			if (Collision::IntersectCylinderVsCylinder(
				wall1->GetPosition(),
				wall1->GetRadius(),
				wall1->GetHeight(),
				wall2->GetPosition(),
				wall2->GetRadius(),
				wall2->GetHeight(),
				outPosition
			))
			{
				// 押し出しの後の位置設定
				wall2->SetPosition(outPosition);
			}
		}
	}
}
