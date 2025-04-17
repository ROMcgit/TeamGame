#include "Easing.h"
#include <corecrt_math.h>
#include <corecrt_math_defines.h>

//! �C�[�W���O(�⊮����)
float Easing::Linear(float start, float end, float t)
{
	return start + t * (end - start);
}

//! �C�[�Y�C��(����)
float Easing::EaseIn(float start, float end, float t)
{
	t = t * t; // ���� (�񎟓I�ȕω�)
	return start + t * (end - start);
}

//! �C�[�Y�A�E�g(����)
float Easing::EaseOut(float start, float end, float t)
{
	t = 1 - (1 - t) * (1 - t); // ���� (�t��EaseIn)
	return start + t * (end - start);
}

//! �C�[�Y�C���A�E�g(����������)
float Easing::EaseInOut(float start, float end, float t)
{
	if (t < 0.5) {
		t = 2 * t * t; // �O��: ����
	}
	else {
		t = 1 - pow(-2 * t + 2, 2) / 2; // �㔼: ����
	}
	return start + t * (end - start);
}

//! �o�b�N(�n�܂��I���ɖ߂��������)
float Easing::EaseBack(float start, float end, float t)
{
	const float s = 1.70158; // �߂�̋���
	t = t * t * ((s + 1) * t - s);
	return start + t * (end - start);
}

//! �o�E���h(�e�ޓ���)
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

//! �G���X�e�B�b�N(�e���̂悤�ȓ���)
float Easing::EaseElastic(float start, float end, float t)
{
	const float c4 = (2 * M_PI) / 3;
	if (t == 0) return start;
	if (t == 1) return end;
	t = -pow(2, 10 * t - 10) * sin((t * 10 - 10.75) * c4);
	return start + t * (end - start);
}

//! �C�[�Y�C���T�[�N��(�~�`����)
float Easing::EaseInCircle(float start, float end, float t)
{
	t = 1 - sqrt(1 - t * t);
	return start + t * (end - start);
}

//! �C�[�Y�A�E�g�T�[�N��(�~�`����)
float Easing::EaseOutCircle(float start, float end, float t)
{
	t = sqrt(1 - (t - 1) * (t - 1));
	return start + t * (end - start);
}

//! �C�[�Y�C���A�E�g�T�[�N��(�~�`�������~�`����)
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
