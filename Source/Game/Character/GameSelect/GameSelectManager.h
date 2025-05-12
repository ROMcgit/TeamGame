#pragma once

#include <vector>
#include"GameSelect.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// �G�l�~�[�}�l�[�W���[
class GameSelectManager
{
private:
	GameSelectManager();
	~GameSelectManager();

public:
	// �B��̃C���X�^���X�擾
	static GameSelectManager& Instance()
	{
		static GameSelectManager instance;
		return instance;
	}

	// �Q�[���Z���N�g���擾
	int GetGameSelectCount() const { return static_cast<int>(gameSelects.size()); }

	// �Q�[���Z���N�g�擾
	std::unique_ptr<GameSelect>& GetGameSelect(int index) {
		if (index < 0 || index >= static_cast<int>(gameSelects.size()))
		{
			throw std::out_of_range("GameSelect index is out of range");
		}
		return gameSelects[index];
	}

	// �X�V����
	void Update(float elapsedTime);

	// �G�l�~�[�̓o�^
	void Register(std::unique_ptr<GameSelect> enemy);

	// �G�l�~�[�폜
	void Remove(GameSelect* enemy);

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

	std::vector<std::unique_ptr<GameSelect>> gameSelects;
	std::vector<GameSelect*> removes;
};