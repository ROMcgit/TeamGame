#pragma once

#include "Game/Character/Player/Player5_AsibaWatari.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"

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
	// ���[�r�[�X�V����
	void UpdateMovie(float elapsedTime);

	// �V�[���؂�ւ�����
	void SceneChange();

private:
	std::unique_ptr<Fade> fade;
	bool setFade = false;
	bool gameClear = false;

	std::unique_ptr <Player5_AsibaWatari> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���
	std::unique_ptr<Sprite> backGround;

	bool movieScene = false; // ���[�r�[����
	float movieTime = 0.0f;  // ���[�r�[����
	int   movieStep = 0;     // ���[�r�[�X�e�b�v
};