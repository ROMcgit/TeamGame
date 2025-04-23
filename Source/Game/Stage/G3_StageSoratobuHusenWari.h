#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// ステージ
class G3_StageSoratobuHusenWari : public Stage
{
public:
	G3_StageSoratobuHusenWari();
	~G3_StageSoratobuHusenWari();

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// デバッグGUI
	void DrawDebugGUI() override;

private:
	// 位置更新処理
	void UpdatePosition(float elapsedTime);

private:
	std::unique_ptr<Model> model;
};