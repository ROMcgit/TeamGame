#pragma once

#include "Sprite.h"
#include <memory>

// �O���錾
class Graphics;
class Fade;

class Pause
{
public:
	Pause();
	~Pause();

	// �|�[�Y��ʂ̕s�����x�擾
	float GetPauseOpacity() { return pauseOpacity; }

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Graphics& graphics);

private:
	// �|�[�Y��ʂ̏���
	void UpdatePause(float elapsedTime);

	// �V�[���؂�ւ�����
	void SceneChange();

private:
	std::unique_ptr<Fade>   fade;
	std::unique_ptr<Sprite> pauseBG;    // �|�[�Y��ʂ̔w�i
	std::unique_ptr<Sprite> pauseSelectImage[2]; // �|�[�Y�I�����
	std::unique_ptr<Sprite> pauseExplanation; // �|�[�Y��ʂ̐���
	std::unique_ptr<Sprite> pauseButton;

	/*! ���ʕϐ� */
	bool  pause = false; // �|�[�Y����
	float pauseOpacity = 0.0f;  // �|�[�Y�摜�̕s�����x
	float pauseOpacitySpeed = 5.0f;   // �|�[�Y��ʂ̕s�����x��ς���X�s�[�h
	bool  setFade = false; // �t�F�[�h��ݒ肵����

	int pauseSelect = 1;
};