#pragma once

#include <vector>
#include "Stage.h"

class StageManager
{
private:
	StageManager();
	~StageManager();

public:
	// �B��̃C���X�^���X�擾
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	// �X�e�[�W�擾
	std::unique_ptr<Stage>& GetStage(int index) {
		if (index < 0 || index >= static_cast<int>(stages.size()))
		{
			throw std::out_of_range("Stage index is out of range");
		}
		return stages[index];
	}

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// �X�e�[�W�o�^
	void Register(std::unique_ptr<Stage> stage);

	// �X�e�[�W��j��
	void Remove(Stage* stage);

	// �X�e�[�W��S�폜
	void Clear();

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// �f�o�b�O�pGUI�`��
	void DrawDebugGUI();

private:
	std::vector<std::unique_ptr<Stage>> stages;
};