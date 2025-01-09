#pragma once
#include "Scene.h"
#include <memory>

// �V�[���}�l�[�W���[
class SceneManager
{
private:
	SceneManager() {};
	~SceneManager() {};

public:
	// �B��̃C���X�^���X�擾
	static SceneManager& Instance()
	{
		static SceneManager instace;
		return instace;
	}

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render();

	// �V�[���N���A
	void Clear();

	// �V�[���؂�ւ�
	void ChangeScene(std::unique_ptr<Scene> scene);

private:
	std::unique_ptr<Scene> currentScene;
	std::unique_ptr<Scene> nextScene;
};