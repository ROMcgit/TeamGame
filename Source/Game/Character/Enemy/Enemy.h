#pragma once

#include "Graphics/Shader.h"
#include "Game/Character/Character.h"
#include "Graphics/Graphics.h"

// エネミー
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2Dスプライトの描画処理
	virtual void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) {};

	// デバッグ用GUI描画
	virtual void DrawDebugGUI() {};

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	void Destroy();

	void Destroy1();

	void Destroy2();

	void Destroy3();

	// ターゲットの方を向く
	void DirectionToTarget(float elapsedTime, float speedRate);

	// 目標地点向きながら移動
	void MoveToTarget(float elapsedTime, float speedRate);

protected:
	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };     // ターゲットの位置
	float moveSpeed = 1.0f;                             // 移動速度
	float turnSpeed = DirectX::XMConvertToRadians(360); // 回転速度
};