#pragma once

class Easing
{
public:
	// ���j�A(�⊮����)
	static float Linear(float start, float end, float t);

	// �C�[�Y�C��(����)
	static float EaseIn(float start, float end, float t);

	// �C�[�Y�A�E�g(����)
	static float EaseOut(float start, float end, float t);

	// �C�[�Y�C���A�E�g(����������)
	static float EaseInOut(float start, float end, float t);

	// �o�b�N(�n�܂��I���ɖ߂��������)
	static float EaseBack(float start, float end, float t);

	// �o�E���h(�e�ޓ���)
	static float EaseBounce(float start, float end, float t);
};