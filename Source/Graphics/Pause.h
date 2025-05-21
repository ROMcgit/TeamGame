#pragma once

#include "Sprite.h"
#include <memory>

// 前方宣言
class Graphics;
class Fade;

class Pause
{
public:
	Pause();
	~Pause();

	// ポーズ画面の不透明度取得
	float GetPauseOpacity() { return pauseOpacity; }

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Graphics& graphics);

private:
	// ポーズ画面の処理
	void UpdatePause(float elapsedTime);

	// シーン切り替え処理
	void SceneChange();

private:
	std::unique_ptr<Fade>   fade;
	std::unique_ptr<Sprite> pauseBG;    // ポーズ画面の背景
	std::unique_ptr<Sprite> pauseSelectImage[2]; // ポーズ選択画面
	std::unique_ptr<Sprite> pauseExplanation; // ポーズ画面の説明
	std::unique_ptr<Sprite> pauseButton;

	/*! 共通変数 */
	bool  pause = false; // ポーズ中か
	float pauseOpacity = 0.0f;  // ポーズ画像の不透明度
	float pauseOpacitySpeed = 5.0f;   // ポーズ画面の不透明度を変えるスピード
	bool  setFade = false; // フェードを設定したか

	int pauseSelect = 1;
};