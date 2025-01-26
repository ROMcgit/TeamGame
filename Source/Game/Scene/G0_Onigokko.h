#pragma once

#include "Game/Character/Player/Player0_Onigokko.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Timer.h"
#include "Graphics/Fade.h"

// �Q�[���V�[��
class G0_Onigokko : public Scene
{
public:
	G0_Onigokko() {}
	~G0_Onigokko() override {}

	// �B��̃C���X�^���X�擾
	static G0_Onigokko& Instance()
	{
		static G0_Onigokko instance;
		return instance;
	}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

	// ���[�r�[�V�[����ݒ�
	void SetMovieScene(bool movieScene) { this->movieScene = movieScene; }

	// ���[�r�[�V�[�������擾
	bool GetMovieScene() { return movieScene; }

private:
	// �v���C���[�̈ʒu����
	void PlayerPositionControll();

	// �J�����̃��[�r�[�X�V����
	void UpdateCameraMovie(float elapsedTime);

private:
	DirectX::XMFLOAT3 target = { 0, 0, 0 };

	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>    renderTarget; //! �����_�[�^�[�Q�b�g
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Timer>  timer;

	static bool  movieScene; // ���[�r�[�V�[����
	float cameraMovieTime = 0.0f; // �J�����̃��[�r�[����

	bool movieFade = false;
	float movieWaitTimer = 0.0f;
	bool gameFinishFade = false;

	bool nextOnideathFade = false;

	std::unique_ptr<Fade> fade;
	bool oniDeathFade = false;

	enum class CameraMovieScene
	{
		OniEntry, // �S�o��
		OniMove,  // �S�ړ�
		OniDeath, // �S���S
	}cameraMovieScene;
};