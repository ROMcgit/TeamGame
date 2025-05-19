#pragma once

#include "Game/Character/Player/Player5_AsibaWatari.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"
#include "Graphics/Pause.h"
#include "Game/Sky.h"

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

	// �X�e�[�W��������
	void NewStage(float elapsedTime);

	// �V�[���؂�ւ�����
	void SceneChange();

public:
	static bool gameClear;

private:
	std::unique_ptr<Fade> fade;
	bool setFade = false;

	std::unique_ptr<Pause> pause;

	std::unique_ptr<Sky> sky;

	std::unique_ptr <Player5_AsibaWatari> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���
	std::unique_ptr<Sprite> backGround;
	DirectX::XMFLOAT3 backGroundColor = { 0, 0, 0 };

	std::unique_ptr<Sprite> actionExplanation;

	bool movieScene = false; // ���[�r�[����
	float movieTime = 0.0f;  // ���[�r�[����
	int   movieStep = 0;     // ���[�r�[�X�e�b�v

	float gameTimer = 0.0f;

	int stageStep = 0;

	bool newStageFinish = false;
};