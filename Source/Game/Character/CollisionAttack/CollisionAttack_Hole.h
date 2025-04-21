#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// ��
class CollisionAttack_Hole : public CollisionAttack
{
public:
	CollisionAttack_Hole(CollisionAttackManager* manager);
	~CollisionAttack_Hole() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// �f�o�b�OGUI�`��
	void DrawDebugGUI() override;

private:
	std::unique_ptr<Model>  model;
};