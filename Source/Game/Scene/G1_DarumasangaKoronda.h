#pragma once

#include "Game/Character/Player/Player1_DarumasangaKoronda.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"
#include "Graphics/Timer.h"

// �Q�[���V�[��
class G1_DarumasangaKoronda : public Scene
{
public:
	G1_DarumasangaKoronda() {}
	~G1_DarumasangaKoronda() override {}

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

	// �v���C���[�̈ʒu����
	void PlayerPositionControll();

public:
	static bool movieScene; // ���[�r�[����

private:
	std::unique_ptr<Fade> fade;
	bool setFade = false;
	bool sceneChange = false;

	std::unique_ptr <Player1_DarumasangaKoronda> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���
	std::unique_ptr<Sprite> backGround;

	float movieTime = 0.0f; // ���[�r�[����
	int   movieStep = 0;    // ���[�r�[�X�e�b�v

	std::unique_ptr<Timer> timer;
};