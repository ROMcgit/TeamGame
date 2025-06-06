#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// 雲
class CollisionAttack_Tataki : public CollisionAttack
{
public:
	CollisionAttack_Tataki(CollisionAttackManager* manager);
	~CollisionAttack_Tataki() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグGUI描画
	void DrawDebugGUI() override;

private:
	// 敵との当たり
	void CollisionVsEnemy();

private:
	std::unique_ptr<Model>  model;
};