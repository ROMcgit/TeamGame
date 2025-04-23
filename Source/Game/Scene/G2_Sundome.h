#pragma once

#include "Game/Character/Player/Player2_Sundome.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"

// �Q�[���V�[��
class G2_Sundome : public Scene
{
public:
	G2_Sundome() {}
	~G2_Sundome() override {}

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

private:
	std::unique_ptr<Fade> fade; // �t�F�[�h

	std::unique_ptr <Player2_Sundome> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���
	std::unique_ptr<Sprite> backGround;

	bool movieScene = false; // ���[�r�[����

	int movieStep   = 0;
	float movieTime = 0.0f;
};