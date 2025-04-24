#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// 雲
class CollisionAttack_Cloud : public CollisionAttack
{
public:
	CollisionAttack_Cloud(CollisionAttackManager* manager);
	~CollisionAttack_Cloud() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグGUI描画
	void DrawDebugGUI() override;

private:
	// 状態更新処理
	void UpdateState();

	// プレイヤーとの接触
	void CollisionVsPlayer();

private:
	std::unique_ptr<Model>  model;
};