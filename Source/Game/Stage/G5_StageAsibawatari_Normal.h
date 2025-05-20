#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// ステージ
class G5_StageAsibawatari_Normal : public Stage
{
public:
	G5_StageAsibawatari_Normal();
	~G5_StageAsibawatari_Normal();

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// デバッグGUI
	void DrawDebugGUI() override;

	//
	void SetMoveDown() { this->moveDown = true; }

private:
	// 移動処理
	void UpdateMove(float elapsedTime);

private:
	std::unique_ptr<Model> model;

	bool moveDown = false;
	float moveTimer = 0.0f;
};