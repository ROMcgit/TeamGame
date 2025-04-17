#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"

bool EnemyManager::enemyStop = false;

EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
	Clear();
}

// �X�V����
void EnemyManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		if(!enemyStop)
		enemy->Update(elapsedTime);
	}

	for (Enemy* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<Enemy>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();
}

// �`�揈��
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2D�X�v���C�g�̕`�揈��
void EnemyManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// �G�l�~�[�o�^
void EnemyManager::Register(std::unique_ptr<Enemy> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// �G�l�~�[�S�폜
void EnemyManager::Clear()
{
	enemies.clear();
}

void EnemyManager::Remove(Enemy* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}

// �f�o�b�O�pGUI�`��
void EnemyManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}