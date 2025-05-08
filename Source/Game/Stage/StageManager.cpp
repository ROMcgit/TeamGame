#include "StageManager.h"
#include <limits>

StageManager::StageManager()
{
}

// デストラクタで全てのステージをクリアする
StageManager::~StageManager()
{
}

// 更新処理
void StageManager::Update(float elapsedTime)
{
	for (const auto& stage : stages)
	{
		stage->Update(elapsedTime);
	}

	for (Stage* stage : removes)
	{
		stages.erase(
			std::remove_if(stages.begin(), stages.end(),
				[stage](const std::unique_ptr<Stage>& ptr) { return ptr.get() == stage; }),
			stages.end()
		);
	}

	removes.clear();
}

// 描画処理
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (const auto& stage : stages)
	{
		stage->Render(dc, shader);
	}
}

// ステージ登録
void StageManager::Register(std::unique_ptr<Stage> stage)
{
	stages.emplace_back(std::move(stage));
}

// ステージを破棄
void StageManager::Remove(Stage* stage)
{
	// 破棄リストに追加
	removes.push_back(stage);
}

// ステージを全削除
void StageManager::Clear()
{
	stages.clear();
}

// レイキャスト
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	HitResult tempHit;
	bool firstHit = true;

	for (const auto& stage : stages)
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
