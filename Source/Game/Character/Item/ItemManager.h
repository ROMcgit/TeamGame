#pragma once

#include <vector>
#include"Item.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// �G�l�~�[�}�l�[�W���[
class ItemManager
{
private:
	ItemManager();
	~ItemManager();

public:
	// �B��̃C���X�^���X�擾
	static ItemManager& Instance()
	{
		static ItemManager instance;
		return instance;
	}

	// �G�l�~�[���擾
	int GetItemCount() const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	std::unique_ptr<Item>& GetItem(int index) {
		if (index < 0 || index >= static_cast<int>(enemies.size()))
		{
			throw std::out_of_range("Item index is out of range");
		}
		return enemies[index];
	}

	// �X�V����
	void Update(float elapsedTime);

	// �G�l�~�[�̓o�^
	void Register(std::unique_ptr<Item> enemy);

	// �G�l�~�[�폜
	void Remove(Item* enemy);

	// �f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// 2D�X�v���C�g�̕`�揈��
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// �G�l�~�[�S�폜
	void Clear();

private:

	std::vector<std::unique_ptr<Item>> enemies;
	std::vector<Item*> removes;
};