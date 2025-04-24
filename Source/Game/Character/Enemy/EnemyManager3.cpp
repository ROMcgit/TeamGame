#include "Game/Character/Enemy/EnemyManager3.h"
#include "Other/Collision.h"

bool EnemyManager3::enemyStop = false;

EnemyManager3::EnemyManager3()
{
}

EnemyManager3::~EnemyManager3()
{
	Clear();
}

// �X�V����
void EnemyManager3::Update(float elapsedTime)
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
void EnemyManager3::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2D�X�v���C�g�̕`�揈��
void EnemyManager3::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager3::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// �G�l�~�[�o�^
void EnemyManager3::Register(std::unique_ptr<Enemy> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// �G�l�~�[�S�폜
void EnemyManager3::Clear()
{
	enemies.clear();
}

void EnemyManager3::Remove(Enemy* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}

// �f�o�b�O�pGUI�`��
void EnemyManager3::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}