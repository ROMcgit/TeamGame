#include "Game/Character/Enemy/EnemyManager1.h"
#include "Other/Collision.h"

bool EnemyManager1::enemyStop = false;

EnemyManager1::EnemyManager1()
{
}

EnemyManager1::~EnemyManager1()
{
	Clear();
}

// �X�V����
void EnemyManager1::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		if (!enemyStop)
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
void EnemyManager1::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2D�X�v���C�g�̕`�揈��
void EnemyManager1::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager1::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// �G�l�~�[�o�^
void EnemyManager1::Register(std::unique_ptr<Enemy> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// �G�l�~�[�S�폜
void EnemyManager1::Clear()
{
	enemies.clear();
}

void EnemyManager1::Remove(Enemy* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}

// �f�o�b�O�pGUI�`��
void EnemyManager1::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}