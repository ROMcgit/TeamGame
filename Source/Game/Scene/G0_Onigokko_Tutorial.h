#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// �S�������`���[�g���A��
class G0_Onigokko_Tutorial : public Scene
{
public:
	G0_Onigokko_Tutorial() {};
	~G0_Onigokko_Tutorial() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	// �摜�̍X�V����
	void SpriteDirector(float elapsedTime);

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> tutorialSpriteFrame;
	std::unique_ptr<Sprite> tutorialSprite[3];
	DirectX::XMFLOAT2       tutorialSpritePos[3];
	DirectX::XMFLOAT2       startPos[3];
	DirectX::XMFLOAT2       endPos[3];
	int tutorialNum = 0;

	int   directorStep = 0;    // ���o�̃X�e�b�v
	float directorTime = 0.0f; // ���o�̎���

	std::unique_ptr<Fade> fade;
	bool setFade = false; // �t�F�[�h��ݒ肵����

	bool tutorialFinish = false;
};