#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "EnemyProjec.h"

// スライム
class EnemyProjec : public Enemy
{
public:
	EnemyProjec();
	~EnemyProjec() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

private:

	// 位置調整
	void PositionControll();

	// ノードとプレイヤーの衝突処理
	void CollisionNodeVsPlayer(const char* nodeName, float nodeRadius);

private:
	Model* model = nullptr;

	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;

	float positionRandamuL = 0;
	float positionRandamuR = 0;

	int waitL = 0;
	int waitR = 0;

	bool moveR = false;
};