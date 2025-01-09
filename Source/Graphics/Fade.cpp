#include "Fade.h"

// コンストラクタ
Fade::Fade()
{
    fadeSprite = std::make_unique<Sprite>();
}

// デストラクタ
Fade::~Fade()
{
}

// 更新処理
bool Fade::Update(float elapsedTime)
{
    if (fade)
    {
        // 不透明度が指定した値までいったら
        if (fadeIn && fadeOpacity > toFadeOpacity || !fadeIn && fadeOpacity < toFadeOpacity)
        {
            fadeOpacity = toFadeOpacity; // 不透明度を指定した値にする
            fade = false;                // フェードを終了
        }
           

        // 待ち時間になったら
        if (waitTime <= 0.0f)
        {
            // フェードインなら足す、じゃないなら引く
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

// 描画処理
void Fade::Render(ID3D11DeviceContext* dc, Graphics& graphics)
{
    float screenWidth = static_cast<float>(graphics.GetScreenWidth());
    float screenHeight = static_cast<float>(graphics.GetScreenHeight());

    // 画面フェード描画
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