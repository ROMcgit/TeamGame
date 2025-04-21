#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// プロペラ
class CollisionAttack_Propeller : public CollisionAttack
{
public:
	CollisionAttack_Propeller(CollisionAttackManager* manager);
	~CollisionAttack_Propeller() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグGUI描画
	void DrawDebugGUI() override;

private:
	// 移動更新処理
	void UpdatePosition(float elapsedTime);

private:
	std::unique_ptr<Model>  model;
};