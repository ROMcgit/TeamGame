#include "Fade.h"
#include "Graphics.h"
#include "Other/Easing.h"
#include <algorithm>

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
    if (!fade)
        return false;

    //! �҂����Ԃ��I����Ă���Ȃ�
    if (waitTime < 0)
    {
        // �t�F�[�h���ԂɊ�Â��Đi�s�� t ���v�Z
        currentTime += std::clamp(elapsedTime, 0.0f, 1.0f);
        // �i�s�� t �̌v�Z�i0�`1�͈̔́j
        float t = std::clamp(currentTime / fadeTime, 0.0f, 1.0f); // 0.0�`1.0 �͈̔͂ɐ���

        // �s�����x����`��ԂōX�V
        fadeOpacity = Easing::Linear(startFadeOpacity, toFadeOpacity, t);

        // �s�����x���ڕW�l�ɒB������t�F�[�h������
        if (t >= 1.0f) // �s�����x�� 1.0�i�܂��͖ڕW�l�j�ɓ��B������
        {
            fadeOpacity = toFadeOpacity;
            fade = false; // �t�F�[�h������
        }
    }
    else
        // �҂����Ԃ����炷
        waitTime -= elapsedTime;

    return true;
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
        screenWidth, screenHeight,
        0,
        color.x,
        color.y,
        color.z,
        fadeOpacity);
}