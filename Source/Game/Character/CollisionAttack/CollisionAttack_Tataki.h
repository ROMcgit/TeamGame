#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// �_
class CollisionAttack_Tataki : public CollisionAttack
{
public:
	CollisionAttack_Tataki(CollisionAttackManager* manager);
	~CollisionAttack_Tataki() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// �f�o�b�OGUI�`��
	void DrawDebugGUI() override;

private:
	// �G�Ƃ̓�����
	void CollisionVsEnemy();

private:
	std::unique_ptr<Model>  model;
};