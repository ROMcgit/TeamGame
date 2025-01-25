#include "Fade.h"
#include "Graphics.h"
#include "Other/Easing.h"
#include <algorithm>

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
    if (!fade)
        return false;

    //! 待ち時間が終わっているなら
    if (waitTime < 0)
    {
        // フェード時間に基づいて進行率 t を計算
        currentTime += std::clamp(elapsedTime, 0.0f, 1.0f);
        // 進行率 t の計算（0～1の範囲）
        float t = std::clamp(currentTime / fadeTime, 0.0f, 1.0f); // 0.0～1.0 の範囲に制限

        // 不透明度を線形補間で更新
        fadeOpacity = Easing::Linear(startFadeOpacity, toFadeOpacity, t);

        // 不透明度が目標値に達したらフェードを解除
        if (t >= 1.0f) // 不透明度が 1.0（または目標値）に到達したら
        {
            fadeOpacity = toFadeOpacity;
            fade = false; // フェードを解除
        }
    }
    else
        // 待ち時間を減らす
        waitTime -= elapsedTime;

    return true;
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
        screenWidth, screenHeight,
        0,
        color.x,
        color.y,
        color.z,
        fadeOpacity);
}