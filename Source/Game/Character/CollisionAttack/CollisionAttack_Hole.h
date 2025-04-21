#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// 穴
class CollisionAttack_Hole : public CollisionAttack
{
public:
	CollisionAttack_Hole(CollisionAttackManager* manager);
	~CollisionAttack_Hole() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグGUI描画
	void DrawDebugGUI() override;

private:
	std::unique_ptr<Model>  model;
};