#pragma once

#include <vector>
#include"Enemy.h"
#include <set>
#include <stdexcept>
#include "Graphics/Graphics.h"

// �G�l�~�[�}�l�[�W���[
class EnemyManager1
{
private:
	EnemyManager1();
	~EnemyManager1();

public:
	// �B��̃C���X�^���X�擾
	static EnemyManager1& Instance()
	{
		static EnemyManager1 instance;
		return instance;
	}

	// �G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	std::unique_ptr<Enemy>& GetEnemy(int index) {
		if (index < 0 || index >= static_cast<int>(enemies.size()))
		{
			throw std::out_of_range("Enemy index is out of range");
		}
		return enemies[index];
	}

	// �X�V����
	void Update(float elapsedTime);

	// �G�l�~�[�̓o�^
	void Register(std::unique_ptr<Enemy> enemy);

	// �G�l�~�[�폜
	void Remove(Enemy* enemy);

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

public:
	static bool enemyStop; // �G���~�߂邩

public:

	std::vector<std::unique_ptr<Enemy>> enemies;
	std::vector<Enemy*> removes;
};