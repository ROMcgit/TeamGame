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

	// エラスティック(弾性のような動き)
	static float EaseElastic(float start, float end, float t);

	/*! 円形 */

	// イーズインサークル(円形加速)
	static float EaseInCircle(float start, float end, float t);

	// イーズアウトサークル(円形減速)
	static float EaseOutCircle(float start, float end, float t);

	// イーズインアウトサークル(円形加速→円形減速)
	static float EaseInOutCircle(float start, float end, float t);
};