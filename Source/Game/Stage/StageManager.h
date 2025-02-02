#pragma once

#include <vector>
#include "Stage.h"

class StageManager
{
private:
	StageManager();
	~StageManager();

public:
	// 唯一のインスタンス取得
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ステージ登録
	void Register(std::unique_ptr<Stage> stage);

	// ステージを破棄
	void Unregister(Stage* stage);

	// ステージを全削除
	void Clear();

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// デバッグ用GUI描画
	void DrawDebugGUI();

private:
	std::vector<std::unique_ptr<Stage>> stages;
};