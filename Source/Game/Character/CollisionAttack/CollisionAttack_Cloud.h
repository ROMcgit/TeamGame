#pragma once

#include "Graphics/Model.h"
#include "Game/Character/CollisionAttack/CollisionAttack.h"
#include "Game/Effect/Effect.h"

// �_
class CollisionAttack_Cloud : public CollisionAttack
{
public:
	CollisionAttack_Cloud(CollisionAttackManager* manager);
	~CollisionAttack_Cloud() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// �f�o�b�OGUI�`��
	void DrawDebugGUI() override;

private:
	// ��ԍX�V����
	void UpdateState();

	// �v���C���[�Ƃ̐ڐG
	void CollisionVsPlayer();

private:
	std::unique_ptr<Model>  model;
};