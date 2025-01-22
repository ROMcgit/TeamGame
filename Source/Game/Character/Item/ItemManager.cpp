#include "Game/Character/Item/ItemManager.h"
#include "Other/Collision.h"

ItemManager::ItemManager()
{
}

ItemManager::~ItemManager()
{
	Clear();
}

// �X�V����
void ItemManager::Update(float elapsedTime)
{
	for (auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}


	for (Item* enemy : removes)
	{
		auto it = std::remove_if(enemies.begin(), enemies.end(),
			[enemy](const std::unique_ptr<Item>& ptr) { return ptr.get() == enemy; });

		if (it != enemies.end())
		{
			enemies.erase(it, enemies.end());
		}
	}

	removes.clear();
}

// �`�揈��
void ItemManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& enemy : enemies)
	{
		enemy->Render(context, shader);
	}
}

// 2D�X�v���C�g�̕`�揈��
void ItemManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& enemy : enemies)
	{
		
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void ItemManager::DrawDebugPrimitive()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// �G�l�~�[�o�^
void ItemManager::Register(std::unique_ptr<Item> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// �G�l�~�[�S�폜
void ItemManager::Clear()
{
	enemies.clear();
}

void ItemManager::Remove(Item* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}

// �f�o�b�O�pGUI�`��
void ItemManager::DrawDebugGUI()
{
	for (auto& enemy : enemies)
	{
		enemy->DrawDebugGUI();
	}
}