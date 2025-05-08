#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// ステージ
class G5_StageAsibawatari_Goal : public Stage
{
public:
	G5_StageAsibawatari_Goal();
	~G5_StageAsibawatari_Goal();

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// デバッグGUI
	void DrawDebugGUI() override;

private:
	// 移動処理
	void UpdateMove(float elapsedTime);

private:
	std::unique_ptr<Model> model;
};