#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// ‰_
class CollisionAttack_Tataki : public CollisionAttack
{
public:
	CollisionAttack_Tataki(CollisionAttackManager* manager);
	~CollisionAttack_Tataki() override;

	// XVˆ—
	void Update(float elapsedTime) override;

	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// ƒfƒoƒbƒOGUI•`‰æ
	void DrawDebugGUI() override;

private:
	// “G‚Æ‚Ì“–‚½‚è
	void CollisionVsEnemy();

private:
	std::unique_ptr<Model>  model;
};