#pragma once

#include "Graphics/Shader.h"
#include "Other/Collision.h"
#include "Game/Character/GameObjectBase.h"

// ステージ
class Stage : public GameObjectBase
{
public:
	Stage() {}
	virtual ~Stage() {};

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// レイキャスト
	virtual bool RayCast(const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	// デバッグ用GUI描画
	virtual void DrawDebugGUI() {};

	// ステージを削除
	void Destroy();

protected:
	float noViewTime = 0.1f;
};