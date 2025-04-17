#include "Easing.h"
#include <corecrt_math.h>
#include <corecrt_math_defines.h>

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

//! エラスティック(弾性のような動き)
float Easing::EaseElastic(float start, float end, float t)
{
	const float c4 = (2 * M_PI) / 3;
	if (t == 0) return start;
	if (t == 1) return end;
	t = -pow(2, 10 * t - 10) * sin((t * 10 - 10.75) * c4);
	return start + t * (end - start);
}

//! イーズインサークル(円形加速)
float Easing::EaseInCircle(float start, float end, float t)
{
	t = 1 - sqrt(1 - t * t);
	return start + t * (end - start);
}

//! イーズアウトサークル(円形減速)
float Easing::EaseOutCircle(float start, float end, float t)
{
	t = sqrt(1 - (t - 1) * (t - 1));
	return start + t * (end - start);
}

//! イーズインアウトサークル(円形加速→円形減速)
float Easing::EaseInOutCircle(float start, float end, float t)
{
	if (t < 0.5) {
		t = (1 - sqrt(1 - (2 * t) * (2 * t))) / 2;
	}
	else {
		t = (sqrt(1 - (-2 * t + 2) * (-2 * t + 2)) + 1) / 2;
	}
	return start + t * (end - start);
}
