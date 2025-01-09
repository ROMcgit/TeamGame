#include "Game/Stage/StageManager.h"
#include <limits>

StageManager::StageManager()
{
}

StageManager::~StageManager()
{
	Clear();
}

// 更新処理
void StageManager::Update(float elapsedTime)
{
	for (Stage* stage : stages)
	{
		stage->Update(elapsedTime);
	}
}

// 描画処理
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Stage* stage : stages)
	{
		stage->Render(dc, shader);
	}
}

// ステージ登録
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

// ステージ全削除
void StageManager::Clear()
{
	for (Stage* stage : stages)
	{
		if (stage != nullptr)
		{
			delete stage;
		}
		stages.clear();
	}
}

// レイキャスト
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	HitResult tempHit;
	bool firstHit = true;

	for (Stage* stage : stages)
	{
		if (stage->RayCast(start, end, tempHit))
		{
			// 初めてのヒットまたはより近いヒットの場合に hit を更新
			if (firstHit || tempHit.distance < hit.distance)
			{
				hit = tempHit;
				result = true;
				firstHit = false;
			}
		}
	}

	return result;
}

// デバッグ用GUI描画
void StageManager::DrawDebugGUI()
{
	for (auto& stage : stages)
	{
		stage->DrawDebugGUI();
	}
}