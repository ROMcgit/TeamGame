#include "DirectorManager.h"

// �R���X�g���N�^
DirectorManager::DirectorManager()
{
}

// �f�X�g���N�^
DirectorManager::~DirectorManager()
{
	Clear();
}

// �X�V����
void DirectorManager::Update(float elapsedTime)
{
	// �X�V����
	for (auto& director : directors)
	{
		director->Update(elapsedTime);
	}

	directors.erase(
		std::remove_if(directors.begin(), directors.end(),
			[this](const std::unique_ptr<Director>& director) {
				return removes.find(director.get()) != removes.end();
			}),
		directors.end()
	);

	removes.clear();
}

// �`�揈��
void DirectorManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (auto& director : directors)
	{
		director->Render(dc, shader);
	}
}

// �f�o�b�OGUI�`��
void DirectorManager::DrawDebugGUI()
{
	for (auto& director : directors)
	{
		director->DrawDebugGUI();
	}
}

void DirectorManager::DrawDebugPrimitive()
{
	for (auto& director : directors)
	{
		director->DrawDebugPrimitive();
	}
}

void DirectorManager::Register(std::unique_ptr<Director> director)
{
	directors.emplace_back(std::move(director));
}

void DirectorManager::Clear()
{
	directors.clear();
}

void DirectorManager::Remove(Director* director)
{
	// �j�����X�g�ɒǉ�
	removes.insert(director);
}
