#pragma once

#include "Game/Character/Player/Player0_Onigokko.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Fade.h"

// �Q�[���V�[��
class SceneGameSelect: public Scene
{
public:
	SceneGameSelect() {}
	~SceneGameSelect() override {}

	// �C���X�^���X�擾
	static SceneGameSelect& Instance()
	{
		static SceneGameSelect instance;
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

private:
	//! �v���C���[�̈ʒu����
	void PlayerPositionControll();

public:
	enum class GameSelect
	{
		Onigokko,           // ���ɂ�����
		DarumasangaKoronda, // ����܂��񂪓]��
		Sundome,            // ���~��
		SoratobuHusenWari,  // ���ԕ��D����
		OssanTataki,        // ��������@��
		Asibawatari,        // ����n��
	};

	static GameSelect gameSelect; // �Q�[���I��

	static bool sceneChange; // �V�[���؂�ւ�

private:
	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<RenderTarget>    renderTarget; //! �����_�[�^�[�Q�b�g
	std::unique_ptr<Sprite> backGround;


	std::unique_ptr<Fade> fade;
	bool setFade = false;
};