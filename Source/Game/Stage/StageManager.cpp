#include "Game/Stage/StageManager.h"
#include <limits>

StageManager::StageManager()
{
}

StageManager::~StageManager()
{
	Clear();
}

// �X�V����
void StageManager::Update(float elapsedTime)
{
	for (Stage* stage : stages)
	{
		stage->Update(elapsedTime);
	}
}

// �`�揈��
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Stage* stage : stages)
	{
		stage->Render(dc, shader);
	}
}

// �X�e�[�W�o�^
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

// �X�e�[�W�S�폜
void StageManager::Clear()
{
	for (Stage* stage : stages)
	{
		if (stage != nullptr)
		{
			delete stage;
		}
		stages.clear();
	}
}

// ���C�L���X�g
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	HitResult tempHit;
	bool firstHit = true;

	for (Stage* stage : stages)
	{
		if (stage->RayCast(start, end, tempHit))
		{
			// ���߂Ẵq�b�g�܂��͂��߂��q�b�g�̏ꍇ�� hit ���X�V
			if (firstHit || tempHit.distance < hit.distance)
			{
				hit = tempHit;
				result = true;
				firstHit = false;
			}
		}
	}

	return result;
}

// �f�o�b�O�pGUI�`��
void StageManager::DrawDebugGUI()
{
	for (auto& stage : stages)
	{
		stage->DrawDebugGUI();
	}
}