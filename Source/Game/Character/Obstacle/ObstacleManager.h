#pragma once

#include <vector>
#include"Obstacle.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// ��Q���}�l�[�W���[
class ObstacleManager
{
private:
	ObstacleManager();
	~ObstacleManager();

public:
	// �B��̃C���X�^���X�擾
	static ObstacleManager& Instance()
	{
		static ObstacleManager instance;
		return instance;
	}

	// �G�l�~�[���擾
	int GetObstacleCount() const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	std::unique_ptr<Obstacle>& GetObstacle(int index) {
		if (index < 0 || index >= static_cast<int>(enemies.size()))
		{
			throw std::out_of_range("Obstacle index is out of range");
		}
		return enemies[index];
	}

	// �X�V����
	void Update(float elapsedTime);

	// �G�l�~�[�̓o�^
	void Register(std::unique_ptr<Obstacle> enemy);

	// �G�l�~�[�폜
	void Remove(Obstacle* enemy);

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

	std::vector<std::unique_ptr<Obstacle>> enemies;
	std::vector<Obstacle*> removes;
};