#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// ステージ
// class Stage
class StageMain : public Stage
{
public:
	// Stage()
	StageMain();
	// ~Stage()
	~StageMain();

	// インスタンス取得
	//static Stage& Instance();

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	Model* model = nullptr;
};