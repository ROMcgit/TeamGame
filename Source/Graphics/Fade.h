#pragma once
#include "Sprite.h"
#include <memory>

// 前方宣言
class Graphics;

class Fade
{
public:
	Fade();
	~Fade();

	// フェードの設定
	void SetFade(const DirectX::XMFLOAT3& color, float fadeOpacity, float toFadeOpacity, float fadeTime, float waitTime = 0.0f)
	{
		if (!this->fade)
		{
			this->fade          = true;                                   // フェードをする
			this->color         = color;                                  // 色
			this->fadeOpacity   = this->startFadeOpacity = fadeOpacity;   // 不透明度
			this->toFadeOpacity = toFadeOpacity;                          // 不透明度をどこまで変化させるかの値
			this->fadeTime      = fadeTime;                               // フェードに書ける時間
			this->currentTime   = 0.0f;                                   // 経過時間をリセット
			this->waitTime      = waitTime;                               // 待ち時間

			//! 一定の値を超えないようにする
			if (this->toFadeOpacity < 0.0f) this->toFadeOpacity = 0.0f;
			if (this->toFadeOpacity > 1.0f) this->toFadeOpacity = 1.0f;
		}
	}

	// フェードの解除
	void SetFadeUnlock()
	{
		fade = false; // フェードを解除
		currentTime = 0.0f;  // 経過時間をリセット
	}

	// フェードしてるか取得
	bool GetFade() { return fade; }

	// フェードの不透明度を取得
	float GetFadeOpacity() { return fadeOpacity; }

	// 更新処理
	bool Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Graphics& graphics);

private:
	std::unique_ptr<Sprite> fadeSprite;
	bool fade = false;     // フェード状態か
	DirectX::XMFLOAT3 color = { 0,0,0 }; // 色
	float fadeTime = 0.0f;      // フェード速度
	float currentTime = 0.0f;      // 現在の経過時間
	float waitTime = 0.0f;      //待ち時間

	float fadeOpacity      = 0.0f; // 不透明度
	float startFadeOpacity = 0.0f; // 不透明度の開始の値
	float toFadeOpacity    = 0.0f; // この値まで不透明度を変化させる
};