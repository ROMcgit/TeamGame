#pragma once

#include <vector>
#include "Director.h"
#include <set>

// ���o�}�l�[�W���[
class DirectorManager
{
public:
	DirectorManager();
	~DirectorManager();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// �f�o�b�OGUI�`��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// ���o�o�^
	void Register(std::unique_ptr<Director> director);

	// ���o�S�폜
	void Clear();

	// ���o���擾
	int GetDirectorCount() const { return static_cast<int>(directors.size()); }

	// ���o�擾
	Director* GetDirector(int index) { return directors.at(index).get(); }

	// ���o�폜
	void Remove(Director* director);

private:
	std::vector<std::unique_ptr<Director>> directors;
	std::set<Director*> removes;
};