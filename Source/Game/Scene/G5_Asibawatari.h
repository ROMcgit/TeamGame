#pragma once

#include "Game/Character/Player/Player5_AsibaWatari.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"

// �Q�[���V�[��
class G5_Asibawatari : public Scene
{
public:
	G5_Asibawatari() {}
	~G5_Asibawatari() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	std::unique_ptr <Player5_AsibaWatari> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>    renderTarget; //! �����_�[�^�[�Q�b�g
	std::unique_ptr<Sprite> backGround;
};