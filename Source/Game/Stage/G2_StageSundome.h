#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// ステージ
class G2_StageSundome : public Stage
{
public:
	G2_StageSundome();
	~G2_StageSundome();

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// デバッグGUI
	void DrawDebugGUI() override;

private:
	// 行列更新処理
	void UpdateTransform();

private:
	std::unique_ptr<Model> model;
};