#pragma once

#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/Fade.h"
#include "Game/Character/CollisionAttack/CollisionAttackManager.h"
#include "Graphics/Text.h"

// �Q�[���V�[��
class G3_SoratobuHusenWari : public Scene
{
public:
	G3_SoratobuHusenWari() {}
	~G3_SoratobuHusenWari() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	// ���D��������
	void NewBalloon(float elapsedTime);

	// �_��������
	void NewCloud(float elapsedTime);

	// �X�R�A�X�V����
	void UpdateScore();

	// �V�[���ύX����
	void SceneChange();

public:
	static bool movieScene;
	static int score;

private:
	CollisionAttackManager collisionAttackManager;

	std::unique_ptr<Text> scoreText;
	DirectX::XMFLOAT2 scoreTextPos = { 1240.0f, 0.0f };

	std::unique_ptr<Fade> fade;
	bool setFade = false;

	std::unique_ptr <Player3_SoratobuHusenWari> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���
	std::unique_ptr<Sprite> backGround;

	float newBalloonWaitTime = 0.0f;
	float newCloudWaitTime = 0.0f;
	float gameTimer = 0.0f;
};