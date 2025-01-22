#include "Game/Character/Director/DirectorManager.h"
#include "Other/Collision.h"

DirectorManager::DirectorManager()
{
}

DirectorManager::~DirectorManager()
{
	Clear();
}

// �X�V����
void DirectorManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	//for (Director* enemy : removes)
	//{
	//	// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
	//	std::vector<Director*>::iterator it = std::find(enemies.begin(),
	//		enemies.end(), enemy);

	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	delete enemy;
	//}

	for (Director* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<Director>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();
}

// �`�揈��
void DirectorManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2D�X�v���C�g�̕`�揈��
void DirectorManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		enemy->SpriteRender(dc, graphics);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void DirectorManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// �G�l�~�[�o�^
void DirectorManager::Register(std::unique_ptr<Director> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// �G�l�~�[�S�폜
void DirectorManager::Clear()
{
	enemies.clear();
}

void DirectorManager::Remove(Director* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}

// �f�o�b�O�pGUI�`��
void DirectorManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}