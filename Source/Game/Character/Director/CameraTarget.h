#pragma once

#include "Graphics/Model.h"
#include "Game/Character/Director/SettingsDirector/Director.h"

// カメラのターゲット
class CameraTarget : public Director
{
public:
	CameraTarget(DirectorManager* manager);
	~CameraTarget() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグ用GUI描画
	void DrawDebugGUI() override;

	// 状態設定
	void SetState(const DirectX::XMFLOAT3 direction,
		const DirectX::XMFLOAT3& position,
		float moveSpeed = 25.0f);

private:
	// ターゲットの位置の状態更新処理
	bool UpdateTargetState();

private:
	std::unique_ptr<Model> model;
	float moveSpeed = 0.0f;
	float turnSpeed = DirectX::XMConvertToRadians(180);
};