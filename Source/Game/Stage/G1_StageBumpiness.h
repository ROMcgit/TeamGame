#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// ステージ
class G1_StageBumpiness : public Stage
{
public:
	G1_StageBumpiness();
	~G1_StageBumpiness();

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// デバッグGUI
	void DrawDebugGUI() override;

private:
	std::unique_ptr<Model> model;

	float dist = 0; // 距離
};