#include "Game/Character/GameSelect/GameSelectManager.h"
#include "Other/Collision.h"

GameSelectManager::GameSelectManager()
{
}

GameSelectManager::~GameSelectManager()
{
	Clear();
}

// �X�V����
void GameSelectManager::Update(float elapsedTime)
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->Update(elapsedTime);
	}

	//for (GameSelect* enemy : removes)
	//{
	//	// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
	//	std::vector<GameSelect*>::iterator it = std::find(enemies.begin(),
	//		enemies.end(), enemy);

	//	if (it != enemies.end())
	//	{
	//		enemies.erase(it);
	//	}

	//	delete enemy;
	//}

	for (GameSelect* enemy : removes)
	{
		auto it = std::remove_if(gameSelects.begin(), gameSelects.end(),
			[enemy](const std::unique_ptr<GameSelect>& ptr) { return ptr.get() == enemy; });

		if (it != gameSelects.end())
		{
			gameSelects.erase(it, gameSelects.end());
		}
	}

	removes.clear();
}

// �`�揈��
void GameSelectManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->Render(context, shader);
	}
}

// 2D�X�v���C�g�̕`�揈��
void GameSelectManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->SpriteRender(dc, graphics);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void GameSelectManager::DrawDebugPrimitive()
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->DrawDebugPrimitive();
	}
}

// �G�l�~�[�o�^
void GameSelectManager::Register(std::unique_ptr<GameSelect> enemy)
{
	gameSelects.emplace_back(std::move(enemy));
}

// �G�l�~�[�S�폜
void GameSelectManager::Clear()
{
	gameSelects.clear();
}

void GameSelectManager::Remove(GameSelect* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}

// �f�o�b�O�pGUI�`��
void GameSelectManager::DrawDebugGUI()
{
	for (auto& gameSelect : gameSelects)
	{
		gameSelect->DrawDebugGUI();
	}
}