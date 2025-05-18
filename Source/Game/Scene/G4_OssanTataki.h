#pragma once

#include "Game/Character/Player/Player4_OssanTataki.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Game/Character/CollisionAttack/CollisionAttackManager.h"
#include "Graphics/Fade.h"
#include "Graphics/Text.h"
#include "Graphics/Timer.h"
#include "Graphics/Pause.h"

// �Q�[���V�[��
class G4_OssanTataki : public Scene
{
public:
	G4_OssanTataki() {}
	~G4_OssanTataki() override {}

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

	// �G��������
	void NewEnemy(float elapsedTime);

	// �X�R�A�X�V����
	void UpdateScore();

	// �V�[���؂�ւ�����
	void SceneChange();

public:
	static int score;

private:
	std::unique_ptr<Text> scoreText;
	DirectX::XMFLOAT2 scoreTextPos = { 1240.0f, 0.0f };

	std::unique_ptr<Pause> pause;

	CollisionAttackManager collisionAttackManager;

	std::unique_ptr<Fade> fade;
	bool setFade = false;

	std::unique_ptr <Player4_OssanTataki> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���
	std::unique_ptr<Sprite> backGround;
	DirectX::XMFLOAT3 backGroundColor = { 0.6f, 1, 0.77f };

	bool movieScene = false; // ���[�r�[����

	float movieTime = 0.0f; // ���[�r�[����
	int   movieStep = 0;    // ���[�r�[�X�e�b�v

	bool newEnemy = false;

	std::unique_ptr<Timer> timer;
};