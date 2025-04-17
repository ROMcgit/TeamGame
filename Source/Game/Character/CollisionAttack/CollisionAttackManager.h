#pragma once

#include <vector>
#include "CollisionAttack.h"
#include <set>

// �ՓˍU���}�l�[�W���[
class CollisionAttackManager
{
public:
	CollisionAttackManager();
	~CollisionAttackManager();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// 2D�X�v���C�g�`��
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// �f�o�b�OGUI�`��
	void DrawDebugGUI();

	// �ՓˍU���o�^
	void Register(std::unique_ptr<CollisionAttack> CollisionAttack);

	// �ՓˍU���S�폜
	void Clear();

	// �ՓˍU�����擾
	int GetCollisionAttackCount() const { return static_cast<int>(collisionAttacks.size()); }

	// �ՓˍU���擾
	CollisionAttack* GetCollisionAttack(int index) { return collisionAttacks.at(index).get(); }

	// �ՓˍU���폜
	void Remove(CollisionAttack* CollisionAttack);

public:

private:
	std::vector<std::unique_ptr<CollisionAttack>> collisionAttacks;
	std::set<CollisionAttack*> removes;
};