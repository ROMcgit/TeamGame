#pragma once
#include "Sprite.h"
#include <memory>

class Text
{
public:
	Text();
	~Text();

	// �`�揈��(��̈ʂ܂őΉ�(�^�C���ȂǗp))
	// �y�^�C���Ȃǂ̑���Ƃ��ė��p����ꍇ�z
	// �umeasure�v���ytrue�z�A�uoneNum�v�ɕϐ��Ȃǂ����蓖�Ă�B���̈ʂ́u0�v�ɂ��Ă����B
	// oneNum�̑Ή��͈́F�y0�`9999�z
	// �y�����̕\���Ƃ��ė��p����ꍇ�z
	// �umeasure�v���yfalse�z�ɂ���B
	// ���ꂼ��̈ʂɐ��l�����蓖�Ă�B
	// �u0�`9�v�܂őΉ��B�u0�����v�܂��́u10�ȏ�v���ƃG���[���o�܂��B
	void Render(ID3D11DeviceContext* dc,
		bool measure,					    // �֐��𑪒�Ɏg����
		bool AlignLeft,                     // �������ɂ��邩
		bool zeroViewTho,				    // 0�ł��\�����邩(��̈�)
		bool zeroViewHun, bool zeroViewTen, // 0�ł��\�����邩(�S�̈ʁA�\�̈�)
		int thousand,					    // ���l(��)
		int hundred, int ten, int oneNum,   // ���l(�S�ȍ~)
		float dx, float dy,				    // �ʒu
		float dw, float dh,				    // �傫��
		float angle,					    // �p�x
		float space,					    // �����̊Ԋu
		float r, float g, float b, float a  // �F
	) const;

	// �`�揈��(���̈ʂ܂őΉ�(�X�R�A�p))
	// �y�^�C���Ȃǂ̑���Ƃ��ė��p����ꍇ�z
	//�umeasure�v���ytrue�z�A�uoneNum�v�ɕϐ��Ȃǂ����蓖�Ă�B���̈ʂ́u0�v�ɂ��Ă����B
	// oneNum�̑Ή��͈́F�y0�`99999999�z
	// �y�����̕\���Ƃ��ė��p����ꍇ�z
	// �umeasure�v���yfalse�z�ɂ���B
	// ���ꂼ��̈ʂɐ��l�����蓖�Ă�B
	// �u0�`9�v�܂őΉ��B�u0�����v�܂��́u10�ȏ�v���ƃG���[���o�܂��B
	void Render(ID3D11DeviceContext* dc,
		bool measure,									// �֐��𑪒�Ɏg����
		bool AlignLeft,                                 // �������ɂ��邩
		bool zeroView,									// 0�̎��\�����邩
		int oku,										// ���l(��)
		int thoMan, int hunMan, int tenMan, int man,	// ���l(��)
		int thousand, int hundred, int ten, int oneNum,	// ���l(��ȍ~)
		float dx, float dy,								// �ʒu
		float dw, float dh,								// �傫��
		float angle,									// �p�x
		float space,									// �����̊Ԋu
		float r, float g, float b, float a				// �F
	) const;

private:
	std::unique_ptr<Sprite> textOku[10];      // ���̈�
	std::unique_ptr<Sprite> textThoMan[10]; // �疜�̈�
	std::unique_ptr<Sprite> textTenMan[10]; // �\���̈�
	std::unique_ptr<Sprite> textMan[10];    // ���̈�
	std::unique_ptr<Sprite> textTho[10];    // ��̈�
	std::unique_ptr<Sprite> textH[10];      // �S�̈�
	std::unique_ptr<Sprite> textT[10];      // �\�̈�
	std::unique_ptr<Sprite> textO[10];      // ��̈�
};