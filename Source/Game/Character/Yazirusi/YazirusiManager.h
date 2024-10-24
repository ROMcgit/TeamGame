#pragma once

#include <vector>
#include "Yazirusi.h"
#include <set>

// �e�ۃ}�l�[�W���[
class YazirusiManager
{
public:
	YazirusiManager();
	~YazirusiManager();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// �e�ۓo�^
	void Register(Yazirusi* projectile);

	// �e�ۑS�폜
	void Clear();

	// �e�ې��擾
	int GetYazirusiCount() const { return static_cast<int>(projectiles.size()); }

	// �e�ێ擾
	Yazirusi* GetYazirusi(int index) { return projectiles.at(index); }

	// �e�ۍ폜
	void Remove(Yazirusi* projectile);

private:
	std::vector<Yazirusi*> projectiles;
	std::set<Yazirusi*> removes;
};