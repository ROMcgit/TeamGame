#include "Easing.h"
#include <corecrt_math.h>

//! イージング(補完無し)
float Easing::Linear(float start, float end, float t)
{
	return start + t * (end - start);
}

//! イーズイン(加速)
float Easing::EaseIn(float start, float end, float t)
{
	t = t * t; // 加速 (二次的な変化)
	return start + t * (end - start);
}

//! イーズアウト(減速)
float Easing::EaseOut(float start, float end, float t)
{
	t = 1 - (1 - t) * (1 - t); // 減速 (逆のEaseIn)
	return start + t * (end - start);
}

//! イーズインアウト(加速→減速)
float Easing::EaseInOut(float start, float end, float t)
{
	if (t < 0.5) {
		t = 2 * t * t; // 前半: 加速
	}
	else {
		t = 1 - pow(-2 * t + 2, 2) / 2; // 後半: 減速
	}
	return start + t * (end - start);
}

//! バック(始まりや終わりに戻りを加える)
float Easing::EaseBack(float start, float end, float t)
{
	const float s = 1.70158; // 戻りの強さ
	t = t * t * ((s + 1) * t - s);
	return start + t * (end - start);
}

//! バウンド(弾む動き)
float Easing::EaseBounce(float start, float end, float t)
{
	const float n1 = 7.5625, d1 = 2.75;
	if (t < 1 / d1) {
		t = n1 * t * t;
	}
	else if (t < 2 / d1) {
		t = n1 * (t -= 1.5 / d1) * t + 0.75;
	}
	else if (t < 2.5 / d1) {
		t = n1 * (t -= 2.25 / d1) * t + 0.9375;
	}
	else {
		t = n1 * (t -= 2.625 / d1) * t + 0.984375;
	}
	return start + t * (end - start);
}
