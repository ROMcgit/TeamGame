#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"

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
		enemy->Update(elapsedTime);
	}

	//for (Enemy* enemy : removes)
	//{
	//	// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
	//	std::vector<Enemy*>::iterator it = std::find(enemies.begin(),
	//		enemies.end(), enemy);

	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	delete enemy;
	//}

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

	// �G���m�̏Փˏ���
	CollisionEnemyVsEnemies();
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

void EnemyManager::CollisionEnemyVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		std::unique_ptr<Enemy>& enemy1 = enemyManager.GetEnemy(i);

		for (int j = i + 1; j < enemyCount; ++j)
		{
			std::unique_ptr<Enemy>& enemy2 = enemyManager.GetEnemy(j);

			// �Փˏ���
			DirectX::XMFLOAT3 outPosition;
			//if (Collision::IntersectSphereVsSphere(
			//	enemy1->GetPosition(),
			//	enemy1->GetRadius(),
			//	enemy2->GetPosition(),
			//	enemy2->GetRadius(),
			//	outPosition
			//)
			//{
			//	// �����o���̌�̈ʒu�ݒ�
			//	enemy2->SetPosition(outPosition);
			//}

			if (Collision::IntersectCylinderVsCylinder(
				enemy1->GetPosition(),
				enemy1->GetRadius(),
				enemy1->GetHeight(),
				enemy2->GetPosition(),
				enemy2->GetRadius(),
				enemy2->GetHeight(),
				outPosition
			))
			{
				// �����o���̌�̈ʒu�ݒ�
				enemy2->SetPosition(outPosition);
			}
		}
	}
}
