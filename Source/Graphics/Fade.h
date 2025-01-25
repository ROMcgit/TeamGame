#pragma once
#include "Sprite.h"
#include <memory>

// �O���錾
class Graphics;

class Fade
{
public:
	Fade();
	~Fade();

	// �t�F�[�h�̐ݒ�
	void SetFade(const DirectX::XMFLOAT3& color, float fadeOpacity, float toFadeOpacity, float fadeTime, float waitTime = 0.0f)
	{
		if (!this->fade)
		{
			this->fade          = true;                                   // �t�F�[�h������
			this->color         = color;                                  // �F
			this->fadeOpacity   = this->startFadeOpacity = fadeOpacity;   // �s�����x
			this->toFadeOpacity = toFadeOpacity;                          // �s�����x���ǂ��܂ŕω������邩�̒l
			this->fadeTime      = fadeTime;                               // �t�F�[�h�ɏ����鎞��
			this->currentTime   = 0.0f;                                   // �o�ߎ��Ԃ����Z�b�g
			this->waitTime      = waitTime;                               // �҂�����

			//! ���̒l�𒴂��Ȃ��悤�ɂ���
			if (this->toFadeOpacity < 0.0f) this->toFadeOpacity = 0.0f;
			if (this->toFadeOpacity > 1.0f) this->toFadeOpacity = 1.0f;
		}
	}

	// �t�F�[�h�̉���
	void SetFadeUnlock()
	{
		fade = false; // �t�F�[�h������
		currentTime = 0.0f;  // �o�ߎ��Ԃ����Z�b�g
	}

	// �t�F�[�h���Ă邩�擾
	bool GetFade() { return fade; }

	// �t�F�[�h�̕s�����x���擾
	float GetFadeOpacity() { return fadeOpacity; }

	// �X�V����
	bool Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Graphics& graphics);

private:
	std::unique_ptr<Sprite> fadeSprite;
	bool fade = false;     // �t�F�[�h��Ԃ�
	DirectX::XMFLOAT3 color = { 0,0,0 }; // �F
	float fadeTime = 0.0f;      // �t�F�[�h���x
	float currentTime = 0.0f;      // ���݂̌o�ߎ���
	float waitTime = 0.0f;      //�҂�����

	float fadeOpacity      = 0.0f; // �s�����x
	float startFadeOpacity = 0.0f; // �s�����x�̊J�n�̒l
	float toFadeOpacity    = 0.0f; // ���̒l�܂ŕs�����x��ω�������
};