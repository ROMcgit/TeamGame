#include "Game/Character/Projectile/ProjectileManager.h"

// �R���X�g���N�^
ProjectileManager::ProjectileManager()
{
}

// �f�X�g���N�^
ProjectileManager::~ProjectileManager()
{
	Clear();
}

// �X�V����
void ProjectileManager::Update(float elapsedTime)
{
	// �X�V����
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}

	// �j������
	// ��projectiles�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Projectile* projectile : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(),
			projectiles.end(), projectile);

		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		// �e�ۂ̔j������
		delete projectile;
	}

	removes.clear();
}

// �`�揈��
void ProjectileManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(context, shader);
	}
}

void ProjectileManager::DrawDebugPrimitive()
{
	for (Projectile* projectile : projectiles)
	{
		projectile->DrawDebugPrimitive();
	}
}

void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		if (projectile != nullptr)
		{
			delete projectile;
			projectile = nullptr;
		}
	}
}

void ProjectileManager::Remove(Projectile* projectile)
{
	// �j�����X�g�ɒǉ�
	removes.insert(projectile);
}
