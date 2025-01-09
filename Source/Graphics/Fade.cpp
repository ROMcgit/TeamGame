#include "Fade.h"

// �R���X�g���N�^
Fade::Fade()
{
    fadeSprite = std::make_unique<Sprite>();
}

// �f�X�g���N�^
Fade::~Fade()
{
}

// �X�V����
bool Fade::Update(float elapsedTime)
{
    if (fade)
    {
        // �s�����x���w�肵���l�܂ł�������
        if (fadeIn && fadeOpacity > toFadeOpacity || !fadeIn && fadeOpacity < toFadeOpacity)
        {
            fadeOpacity = toFadeOpacity; // �s�����x���w�肵���l�ɂ���
            fade = false;                // �t�F�[�h���I��
        }
           

        // �҂����ԂɂȂ�����
        if (waitTime <= 0.0f)
        {
            // �t�F�[�h�C���Ȃ瑫���A����Ȃ��Ȃ����
            if (fadeIn)
                fadeOpacity += (fadeSpeed * elapsedTime) * 0.2f;
            else
                fadeOpacity -= (fadeSpeed * elapsedTime) * 0.2f;
        }
        else
            waitTime -= elapsedTime;

        return true;
    }
    else
    {
        return false;
    }
}

// �`�揈��
void Fade::Render(ID3D11DeviceContext* dc, Graphics& graphics)
{
    float screenWidth = static_cast<float>(graphics.GetScreenWidth());
    float screenHeight = static_cast<float>(graphics.GetScreenHeight());

    // ��ʃt�F�[�h�`��
    fadeSprite->Render(dc,
        0, 0,
        screenWidth, screenHeight,
        0, 0,
        1320, 760,
        0,
        color.x,
        color.y,
        color.z,
        fadeOpacity);
}