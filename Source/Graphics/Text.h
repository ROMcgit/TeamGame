#pragma once
#include "Sprite.h"
#include <memory>

class Text
{
public:
	Text();
	~Text();

	/*! �Q�b�^�[ */

	// �ŏ��l
	int GetMin() const { return limit.min; }

	// �ő�l(��̈ʂ܂ł̏ꍇ)
	int GetMaxTho() const { return limit.maxTho; }

	// �ő�l(���̈ʂ܂ł̏ꍇ)
	int GetMaxOku() const { return limit.maxOku; }

	// �`�揈��(��̈ʂ܂őΉ�(�^�C���ȂǗp))
	// �y�^�C���Ȃǂ̑���Ƃ��ė��p����ꍇ�z
	// �umeasure�v���ytrue�z�A�uoneNum�v�ɕϐ��Ȃǂ����蓖�Ă�B���̈ʂ́u0�v�ɂ��Ă����B
	// oneNum�̑Ή��͈́F�y0�`9999�z
	// �y�����̕\���Ƃ��ė��p����ꍇ�z
	// �umeasure�v���yfalse�z�ɂ���B
	// ���ꂼ��̈ʂɐ��l�����蓖�Ă�B
	// �u0�`9�v�܂őΉ��B�u0�����v�܂��́u10�ȏ�v���ƃG���[���o�܂��B
	void RenderThousand(ID3D11DeviceContext* dc,
		bool AlignLeft,                                     // �������ɂ��邩
		int oneNum,                                         // �ϐ������蓖�Ă�
		bool zeroViewTho,				                    // 0�ł��\�����邩(��̈�)
		bool zeroViewHun, bool zeroViewTen,                 // 0�ł��\�����邩(�S�̈ʁA�\�̈�)
		float dx, float dy,				                    // �ʒu
		float dw = 8, float dh = 8,		                    // �傫��
		float angle = 0,					                // �p�x
		float space = 28,					                // �����̊Ԋu
		float r = 1, float g = 1, float b = 1, float a = 1, // �F
		int thousand = 0,					                // ���l(��)
		int hundred = 0, int ten = 0                        // ���l(�S�ȍ~)
	) const;

	// �`�揈��(���̈ʂ܂őΉ�(�X�R�A�p))
	// �y�^�C���Ȃǂ̑���Ƃ��ė��p����ꍇ�z
	//�umeasure�v���ytrue�z�A�uoneNum�v�ɕϐ��Ȃǂ����蓖�Ă�B���̈ʂ́u0�v�ɂ��Ă����B
	// oneNum�̑Ή��͈́F�y0�`99999999�z
	// �y�����̕\���Ƃ��ė��p����ꍇ�z
	// �umeasure�v���yfalse�z�ɂ���B
	// ���ꂼ��̈ʂɐ��l�����蓖�Ă�B
	// �u0�`9�v�܂őΉ��B�u0�����v�܂��́u10�ȏ�v���ƃG���[���o�܂��B
	void RenderOku(ID3D11DeviceContext* dc,
		bool AlignLeft,                                              // �������ɂ��邩
		int oneNum,                                                  // �ϐ������蓖�Ă�
		bool zeroView,							                     // 0�̎��\�����邩
		float dx, float dy,								             // �ʒu
		float dw = 8, float dh = 8,								     // �傫��
		float angle = 0,									         // �p�x
		float space = 28,									         // �����̊Ԋu
		float r = 1, float g = 1, float b = 1, float a = 1,	         // �F
		int oku = 0,										         // ���l(��)
		int thoMan = 0, int hunMan = 0, int tenMan = 0, int man = 0, // ���l(��)
		int thousand = 0, int hundred = 0, int ten = 0               // ���l(��ȍ~)
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

	struct Limit
	{
		int min    = 0;         // �ŏ��l
		int maxTho = 9999;      // �ő�l(��̈ʂ܂ł̏ꍇ)
		int maxOku = 999999999; // �ő�l(���̈ʂ܂ł̏ꍇ)
	}limit;
};