#include "Game/Character/Obstacle/ObstacleManager.h"
#include "Other/Collision.h"

ObstacleManager::ObstacleManager()
{
}

ObstacleManager::~ObstacleManager()
{
	Clear();
}

// �X�V����
void ObstacleManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//for (Obstacle* enemy : removes)
	//{
	//	// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
	//	std::vector<Obstacle*>::iterator it = std::find(enemies.begin(),
	//		enemies.end(), enemy);

	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	delete enemy;
	//}

	for (Obstacle* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<Obstacle>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();

	// �G���m�̏Փˏ���
	CollisionObstacleVsEnemies();
}

// �`�揈��
void ObstacleManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2D�X�v���C�g�̕`�揈��
void ObstacleManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void ObstacleManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// �G�l�~�[�o�^
void ObstacleManager::Register(std::unique_ptr<Obstacle> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// �G�l�~�[�S�폜
void ObstacleManager::Clear()
{
	enemies.clear();
}

void ObstacleManager::Remove(Obstacle* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}

// �f�o�b�O�pGUI�`��
void ObstacleManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}
