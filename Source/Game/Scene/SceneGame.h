#pragma once

#include "Game/Character/Player.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemySlime.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

// �Q�[���V�[��
//class SceneGame
class SceneGame: public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	//Stage* stage = nullptr;
	std::unique_ptr <Player> player;
	std::unique_ptr <CameraController> cameraController;
	std::unique_ptr<Sprite> enemyHp;
};