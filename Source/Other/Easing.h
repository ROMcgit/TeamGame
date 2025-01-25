#pragma once

class Easing
{
public:
	// リニア(補完無し)
	static float Linear(float start, float end, float t);

	// イーズイン(加速)
	static float EaseIn(float start, float end, float t);

	// イーズアウト(減速)
	static float EaseOut(float start, float end, float t);

	// イーズインアウト(加速→減速)
	static float EaseInOut(float start, float end, float t);

	// バック(始まりや終わりに戻りを加える)
	static float EaseBack(float start, float end, float t);

	// バウンド(弾む動き)
	static float EaseBounce(float start, float end, float t);
};