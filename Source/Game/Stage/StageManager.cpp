#include "StageManager.h"
#include <limits>

StageManager::StageManager()
{
}

// �f�X�g���N�^�őS�ẴX�e�[�W���N���A����
StageManager::~StageManager()
{
}

// �X�V����
void StageManager::Update(float elapsedTime)
{
	for (const auto& stage : stages)
	{
		stage->Update(elapsedTime);
	}

	for (Stage* stage : removes)
	{
		stages.erase(
			std::remove_if(stages.begin(), stages.end(),
				[stage](const std::unique_ptr<Stage>& ptr) { return ptr.get() == stage; }),
			stages.end()
		);
	}

	removes.clear();
}

// �`�揈��
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (const auto& stage : stages)
	{
		stage->Render(dc, shader);
	}
}

// �X�e�[�W�o�^
void StageManager::Register(std::unique_ptr<Stage> stage)
{
	stages.emplace_back(std::move(stage));
}

// �X�e�[�W��j��
void StageManager::Remove(Stage* stage)
{
	// �j�����X�g�ɒǉ�
	removes.push_back(stage);
}

// �X�e�[�W��S�폜
void StageManager::Clear()
{
	stages.clear();
}

// ���C�L���X�g
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	HitResult tempHit;
	bool firstHit = true;

	for (const auto& stage : stages)
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
