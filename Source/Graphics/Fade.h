#pragma once
#include "Sprite.h"
#include "Graphics.h"
#include <memory>

class Fade
{
public:
	Fade();
	~Fade();

	// フェードの設定
	void SetFade(DirectX::XMFLOAT3& color, float fadeOpacity, float toFadeOpacity, float fadeSpeed, float waitTime = 0.0f)
	{
		this->fade          = true;
		this->color         = color;         // 色
		this->fadeOpacity   = fadeOpacity;   // 不透明度
		this->toFadeOpacity = toFadeOpacity; // 不透明度をどこまで変化させるかの値
		this->fadeSpeed     = fadeSpeed;     // フェードのスピード
		this->waitTime      = waitTime;      // 待ち時間

		//! 一定の値を超えないようにする
		if (this->toFadeOpacity < 0.0f) this->toFadeOpacity = 0.0f;
		if (this->toFadeOpacity > 1.0f) this->toFadeOpacity = 1.0f;

		//! フェードの数値に合わせて、フェードインかフェードアウトにする
		if (this->fadeOpacity < this->toFadeOpacity) this->fadeIn = true;  // フェードイン
		if (this->fadeOpacity > this->toFadeOpacity) this->fadeIn = false; // フェードアウト
	}

	// フェードしてるか取得
	bool GetFade() { return fade; }

	// 更新処理
	bool Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Graphics& graphics);

private:
	std::unique_ptr<Sprite> fadeSprite;
	bool fade               = false;
	bool fadeIn             = false; // フェードインするか
	DirectX::XMFLOAT3 color = { 0,0,0 }; // 色
	float fadeSpeed         = 0.0f; // フェード速度
	float waitTime = 0.0f;     //待ち時間

	float fadeOpacity = 0.0f;   // 不透明度
	float toFadeOpacity = 0.0f; // この値まで不透明度を変化させる
};