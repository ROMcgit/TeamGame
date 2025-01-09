#pragma once
#include "Sprite.h"
#include "Graphics.h"
#include <memory>

class Fade
{
public:
	Fade();
	~Fade();

	// �t�F�[�h�̐ݒ�
	void SetFade(DirectX::XMFLOAT3& color, float fadeOpacity, float toFadeOpacity, float fadeSpeed, float waitTime = 0.0f)
	{
		this->fade          = true;
		this->color         = color;         // �F
		this->fadeOpacity   = fadeOpacity;   // �s�����x
		this->toFadeOpacity = toFadeOpacity; // �s�����x���ǂ��܂ŕω������邩�̒l
		this->fadeSpeed     = fadeSpeed;     // �t�F�[�h�̃X�s�[�h
		this->waitTime      = waitTime;      // �҂�����

		//! ���̒l�𒴂��Ȃ��悤�ɂ���
		if (this->toFadeOpacity < 0.0f) this->toFadeOpacity = 0.0f;
		if (this->toFadeOpacity > 1.0f) this->toFadeOpacity = 1.0f;

		//! �t�F�[�h�̐��l�ɍ��킹�āA�t�F�[�h�C�����t�F�[�h�A�E�g�ɂ���
		if (this->fadeOpacity < this->toFadeOpacity) this->fadeIn = true;  // �t�F�[�h�C��
		if (this->fadeOpacity > this->toFadeOpacity) this->fadeIn = false; // �t�F�[�h�A�E�g
	}

	// �t�F�[�h���Ă邩�擾
	bool GetFade() { return fade; }

	// �X�V����
	bool Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Graphics& graphics);

private:
	std::unique_ptr<Sprite> fadeSprite;
	bool fade               = false;
	bool fadeIn             = false; // �t�F�[�h�C�����邩
	DirectX::XMFLOAT3 color = { 0,0,0 }; // �F
	float fadeSpeed         = 0.0f; // �t�F�[�h���x
	float waitTime = 0.0f;     //�҂�����

	float fadeOpacity = 0.0f;   // �s�����x
	float toFadeOpacity = 0.0f; // ���̒l�܂ŕs�����x��ω�������
};