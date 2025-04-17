#include "CollisionAttackManager.h"

// �R���X�g���N�^
CollisionAttackManager::CollisionAttackManager()
{
}

// �f�X�g���N�^
CollisionAttackManager::~CollisionAttackManager()
{
	Clear();
}

// �X�V����
void CollisionAttackManager::Update(float elapsedTime)
{
	// �X�V����
	for (auto& CollisionAttack : collisionAttacks)
	{
		CollisionAttack->Update(elapsedTime);
	}

	collisionAttacks.erase(
		std::remove_if(collisionAttacks.begin(), collisionAttacks.end(),
			[this](const std::unique_ptr<CollisionAttack>& CollisionAttack) {
				return removes.find(CollisionAttack.get()) != removes.end();
			}),
		collisionAttacks.end()
	);

	removes.clear();
}

// �`�揈��
void CollisionAttackManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->Render(dc, shader);
	}
}

// 2D�X�v���C�g�`��
void CollisionAttackManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->SpriteRender(dc, graphics);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void CollisionAttackManager::DrawDebugPrimitive()
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->DrawDebugPrimitive();
	}
}

// �f�o�b�OGUI�`��
void CollisionAttackManager::DrawDebugGUI()
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->DrawDebugGUI();
	}
}

// �ՓˍU���o�^
void CollisionAttackManager::Register(std::unique_ptr<CollisionAttack> collisionAttack)
{
	collisionAttacks.emplace_back(std::move(collisionAttack));
}

// �ՓˍU���S�폜
void CollisionAttackManager::Clear()
{
	collisionAttacks.clear();
}

// �ՓˍU���폜
void CollisionAttackManager::Remove(CollisionAttack* collisionAttack)
{
	// �j�����X�g�ɒǉ�
	removes.insert(collisionAttack);
}