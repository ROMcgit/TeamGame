#pragma once

#include "Game/Character/Player/Player4_OssanTataki.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"

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
	std::unique_ptr <Player4_OssanTataki> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���
	std::unique_ptr<Sprite> backGround;
};