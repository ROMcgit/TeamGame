#pragma once

#include "Game/Character/Player/Player0_Onigokko.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/Enemy/EnemyOni.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/ShadowMap.h"
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

	// �Q�[����ʕ`��
	void RenderGameSprite(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	// �{�[�i�X�摜�̕\������
	void UpdateBonusImage(float elapsedTime);


public:
#if 1
	enum class GameSelectA
	{
		GameSelect,
		Onigokko,           // ���ɂ�����
		DarumasangaKoronda, // ����܂��񂪓]��
		Sundome,            // ���~��
		SoratobuHusenWari,  // ���ԕ��D����
		OssanTataki,        // ��������@��
		Asibawatari,        // ����n��
	};

	static GameSelectA gameSelect; // �Q�[���I��

	static bool sceneChange; // �V�[���؂�ւ�

	//! �N���A
	struct Clear
	{
		bool onigokko           = false; // ���ɂ�����
		bool darumasangaKoronda = false; // ����܂��񂪓]��
		bool sundome            = false; // ���~��
		bool soratobuHusenWari  = false; // ���ԕ��D����
		bool ossanTataki        = false; // ��������@��
		bool asibawatari        = false; // ����n��
	};

	static Clear clear;

#endif

private:
	std::unique_ptr <Player0_Onigokko> player;
	std::unique_ptr <CameraController> cameraController;

	std::unique_ptr<Sprite> gameExplanation;
	float gameExplanationOpacity = 1.0f;
	bool gameExplanationOpacityUp = false;

	std::unique_ptr<Sprite> bonusExplanation;

	std::unique_ptr<RenderTarget>  renderTarget; //! �����_�[�^�[�Q�b�g
	ShadowMap                      shadowMap;    // �V���h�E�}�b�v�̎���

	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> gameSelectSprite[6];
	DirectX::XMFLOAT2 gameSelectSpritePos;

	bool viewBonusImage = false;
	std::unique_ptr<Sprite> hint[7];
	std::unique_ptr<Sprite> bonusImage[7];
	std::unique_ptr<Sprite> bonusImageFrame[7];
	DirectX::XMFLOAT3 bonusImageFrameColor = { 0, 0.69f, 1.0f };
	bool gameComplete = false;

	int   bonusImageNum = 1;
	float bonusImageOpacity = 0.0f;
	float bonusImagePosX[7];
	float startBonusImagePosX[7];
	float endBonusImagePosX[7];
	float bonusImageColor[7];
	bool bonusImageMove = false;
	float bonusImageElapsedTime = 0.0f;
	float inputWaitTime = 0.0f;

	std::unique_ptr<Fade> fade;
	bool setFade = false;
};