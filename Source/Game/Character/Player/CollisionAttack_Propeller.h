#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// �v���y��
class CollisionAttack_Propeller : public CollisionAttack
{
public:
	CollisionAttack_Propeller(CollisionAttackManager* manager);
	~CollisionAttack_Propeller() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// �f�o�b�OGUI�`��
	void DrawDebugGUI() override;

private:
	// �ړ��X�V����
	void UpdatePosition(float elapsedTime);

private:
	std::unique_ptr<Model>  model;
};