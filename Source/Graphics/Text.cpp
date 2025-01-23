#include "Text.h"
#include <string>
#include <iostream>

Text::Text()
{
	// �摜�ǂݍ���
	for (int i = 0; i < 10; i++)
	{
		std::string filePath = "Data/Font/" + std::to_string(i) + ".png";
		// ���̈�
		textOku[i]      = std::make_unique<Sprite>(filePath.c_str());
		// �疜�̈�
		textThoMan[i] = std::make_unique<Sprite>(filePath.c_str());
		// �\���̈�
		textTenMan[i] = std::make_unique<Sprite>(filePath.c_str());
		// ���̈�
		textMan[i]    = std::make_unique<Sprite>(filePath.c_str());
		// ��̈�
		textTho[i]    = std::make_unique<Sprite>(filePath.c_str());
		// �S�̈�
		textH[i]      = std::make_unique<Sprite>(filePath.c_str());
		// �\�̈�     
		textT[i]      = std::make_unique<Sprite>(filePath.c_str());
		// ��̈�     
		textO[i]      = std::make_unique<Sprite>(filePath.c_str());
	}
}

Text::~Text()
{
}

// �`�揈��(�S�̈ʂ܂őΉ�(�^�C���p))
void Text::RenderThousand(ID3D11DeviceContext* dc,
	bool AlignLeft,                      // �������ɂ��邩
	int oneNum,                          // �ϐ������蓖�Ă�
	bool zeroViewTho,				     // 0�ł��\�����邩(��̈�)
	bool zeroViewHun, bool zeroViewTen,  // 0�ł��\�����邩(�S�̈ʁA�\�̈�)
	float dx, float dy,				     // �ʒu
	float dw, float dh,		             // �傫��
	float angle,					     // �p�x
	float space,					     // �����̊Ԋu
	float r, float g, float b, float a,  // �F
	int thousand,					     // ���l(��)
	int hundred, int ten                 // ���l(�S�ȍ~)
) const
{
	// bool���̐錾 //
	bool viewTho = false;
	bool viewHun = false;
	bool viewTen = false;

	// �\������ //
	if (!zeroViewTho)
	{
		if (oneNum >= 1000 || thousand > 0) viewTho = true;
		else viewTho = false;
	}
	if (!zeroViewHun)
	{
		if (oneNum >= 100 || hundred > 0) viewHun = true;
		else viewHun = false;
	}
	if (!zeroViewTen)
	{
		if (oneNum >= 10 || ten > 0) viewTen = true;
		else viewTen = false;
	}

/****************************************************************************************************************************/
	
	// �G���[���b�Z�[�W //

	// 0��菬�����܂���999���傫����
	if (oneNum < 0 || oneNum > 9999)
	{
		std::cerr << "Error: ��̈ʂɓ����鐔�l���u0�v�����܂���9999���傫���ł�: " << std::endl;
		throw std::runtime_error("�l�����e�͈͂𒴂��܂����I0�`9999�܂ł͈̔͂ɂ��Ă��������B");
	}
	// ��̈ʁA�܂��͕S�̈ʁA�܂��͏\�̈ʂ�0��菬�����A�܂���9���傫����
	else if (thousand < 0 || hundred < 0 || ten < 0 ||
			thousand > 9 || hundred > 9 || ten > 9)
	{
		std::cerr << "Error: �u�S�̈ʁv�܂��́u�\�̈ʁv���u0�v�����܂��́u10�v�ȏ�ł�:" << std::endl;
		throw std::runtime_error("�l�����e�͈͂𒴂��܂����I0�`9�܂ł͈̔͂ɂ��Ă��������B");
	}

/****************************************************************************************************************************/
	
	// ���l�̒��� //
	thousand = oneNum / 1000;  // ��̈ʂ��擾
	oneNum %= 1000;            // �c��̒l

	hundred = oneNum / 100;   // �S�̈ʂ��擾
	oneNum %= 100;            // �c��̒l

	ten = oneNum / 10;        // �\�̈ʂ��擾
	oneNum %= 10;             // ��̈ʂ��擾
	
/****************************************************************************************************************************/
	
	// ���l�̕\�� //

	// ������ //
	if (AlignLeft)
	{
		// ��̈�
		if (zeroViewTho) //0�ł��\������
		{
			textH[thousand]->Render(dc,
				dx, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTho) //0�̎��\�����Ȃ�
		{
			textH[thousand]->Render(dc,
				dx, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// �S�̈�
		if (zeroViewHun) //0�ł��\������
		{
			textH[hundred]->Render(dc,
				dx + space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewHun) //0�̎��\�����Ȃ�
		{
			textH[hundred]->Render(dc,
				dx + space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// �\�̈�
		if (zeroViewTen) //0�ł��\������
		{
			textT[ten]->Render(dc,
				dx + space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTen) //0�̎��\�����Ȃ�
		{
			textT[ten]->Render(dc,
				dx + space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		textO[oneNum]->Render(dc,
			dx + space * 3, dy,
			dw * 5, dh * 5,
			0, 0,
			82, 82,
			angle,
			r, g, b, a);
	}

	// �E���� //
	if (!AlignLeft)
	{
		// ��̈�
		if (zeroViewTho) //0�ł��\������
		{
			textH[thousand]->Render(dc,
				dx - space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTho) //0�̎��\�����Ȃ�
		{
			textH[thousand]->Render(dc,
				dx - space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// �S�̈�
		if (zeroViewHun) //0�ł��\������
		{
			textH[hundred]->Render(dc,
				dx - space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewHun) //0�̎��\�����Ȃ�
		{
			textH[hundred]->Render(dc,
				dx - space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// �\�̈�
		if (zeroViewTen) //0�ł��\������
		{
			textT[ten]->Render(dc,
				dx - space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTen) //0�̎��\�����Ȃ�
		{
			textT[ten]->Render(dc,
				dx - space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		textO[oneNum]->Render(dc,
			dx, dy,
			dw * 5, dh * 5,
			0, 0,
			82, 82,
			angle,
			r, g, b, a);
	}
}

// �`�揈��(���̈ʂ܂őΉ�(�X�R�A�p))
void Text::RenderOku(ID3D11DeviceContext* dc, 
	bool AlignLeft,                                 // �������ɂ��邩
	int oneNum,                                     // �ϐ������蓖�Ă�
	bool zeroView,									// 0�̎��\�����邩
	float dx, float dy,								// �ʒu
	float dw, float dh,								// �傫��
	float angle,									// �p�x
	float space,									// �����̊Ԋu
	float r, float g, float b, float a,				// �F
	int oku,										// ���l(��)
	int thoMan, int hunMan, int tenMan, int man,	// ���l(��)
	int thousand, int hundred, int ten              // ���l(��ȍ~)
) const
{
	// bool�^�̐錾 //
	bool viewThoMan = false;
	bool viewHunMan = false;
	bool viewTenMan = false;
	bool viewMan    = false;
	bool viewTho    = false;
	bool viewHun    = false;
	bool viewTen    = false;

	// �\������ //
	if (zeroView == false)
	{
		if (oneNum >= 10000000 || thoMan > 0 || oku > 0) viewThoMan = true;
		else viewThoMan = false;

		if (oneNum >= 1000000 || hunMan > 0 || thoMan > 0 || oku > 0)  viewHunMan = true;
		else viewHunMan = false;	

		if (oneNum >= 100000 || tenMan > 0 || hunMan > 0 || thoMan > 0 || oku > 0)
			viewTenMan = true;
		else viewTenMan = false;	

		if (oneNum >= 10000 || man > 0 || tenMan > 0 || hunMan > 0 || thoMan > 0 || oku > 0)
			viewMan = true;
		else viewMan = false;

		if (oneNum >= 1000 || thousand > 0 || man > 0 || tenMan > 0 || hunMan > 0 || thoMan > 0 || oku > 0)
			viewTho = true;
		else viewTho = false;

		if (oneNum >= 100 || hundred > 0 || thousand > 0 || man > 0 || tenMan > 0 || hunMan > 0 || thoMan > 0 || oku > 0)
			viewHun = true;
		else viewHun = false;

		if (oneNum >= 10 || ten > 0 || hundred > 0 || thousand > 0 || man > 0 || tenMan > 0 || hunMan > 0 || thoMan > 0 || oku > 0)
			viewTen = true;
		else viewTen = false;
	}

/****************************************************************************************************************************/
	
	// �G���[���b�Z�[�W //

	// 0��菬�����܂���1���ȏ�̎�
	if (oneNum < 0 || oneNum >= 100000000)
	{
		std::cerr << "Error: ��̈ʂɓ����鐔�l���u0�v�����܂��́u1���v�ȏ�ł�: " << std::endl;
		throw std::runtime_error("�l�����e�͈͂𒴂��܂����I0�`1�������܂ł͈̔͂ɂ��Ă��������B");
	}
	// ���l���͈͊O�̎�
	else if (oku < 0 || thoMan < 0 || hunMan < 0 || tenMan < 0 || man < 0 || thousand < 0 || hundred < 0 || ten < 0 ||
			oku > 9 || thoMan > 9 || hunMan > 9 || tenMan > 9 || man > 9 || thousand > 9 || hundred > 9 || ten > 9)
	{
		std::cerr << "Error: ���蓖�Ă�ꂽ���l���u0�v�����܂��́u10�v�ȏ�ł�:" << std::endl;
		throw std::runtime_error("�l�����e�͈͂𒴂��܂����I0�`9�܂ł͈̔͂ɂ��Ă��������B");
	}

/****************************************************************************************************************************/

	// ���l�̒��� //
	// ���̈�
	oku = oneNum / 100000000;
	oneNum %= 100000000;

	// �疜�̈�
	thoMan = oneNum / 10000000;
	oneNum %= 10000000;

	// �S���̈�
	hunMan = oneNum / 1000000;
	oneNum %= 1000000;

	// �\���̈�
	tenMan = oneNum / 100000;
	oneNum %= 100000;

	// ���̈�
	man = oneNum / 10000;
	oneNum %= 10000;

	// ��̈�
	thousand = oneNum / 1000;
	oneNum %= 1000;

	// �S�̈�
	hundred = oneNum / 100;
	oneNum %= 100;

	// �\�̈�
	ten = oneNum / 10;
	oneNum %= 10;
	
/****************************************************************************************************************************/
	
	// ���l�̕\�� //

	// ������ //
	if (AlignLeft)
	{
		//0�ł��\������
		if (zeroView)
		{
			// ���̈�
			textOku[oku]->Render(dc,
				dx, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �疜�̈�
			textThoMan[thoMan]->Render(dc,
				dx + space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �S���̈�
			textThoMan[hunMan]->Render(dc,
				dx + space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �\���̈�
			textTenMan[tenMan]->Render(dc,
				dx + space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// ���̈�
			textMan[man]->Render(dc,
				dx + space * 4, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// ��̈�
			textTho[thousand]->Render(dc,
				dx + space * 5, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �S�̈�
			textH[hundred]->Render(dc,
				dx + space * 6, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �\�̈�
			textT[ten]->Render(dc,
				dx + space * 7, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 0�̎��\�����Ȃ� //

		else
		{
			// ���̈�
			if (oku != 0)
			{
				textO[oku]->Render(dc,
					dx, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �疜�̈�
			if (viewThoMan)
			{
				textThoMan[thoMan]->Render(dc,
					dx + space, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			if (viewHunMan)
			{
				// �S���̈�
				textThoMan[hunMan]->Render(dc,
					dx + space * 2, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �\���̈�
			if (viewTenMan)
			{
				textTenMan[tenMan]->Render(dc,
					dx + space * 3, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// ���̈�
			if (viewMan)
			{
				textMan[man]->Render(dc,
					dx + space * 4, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}


			// ��̈�
			if (viewTho)
			{
				textTho[thousand]->Render(dc,
					dx + space * 5, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �S�̈�
			if (viewHun)
			{
				textH[hundred]->Render(dc,
					dx + space * 6, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �\�̈�
			if (viewTen) //0�̎��\�����Ȃ�
			{

				textT[ten]->Render(dc,
					dx + space * 7, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}
		}

		// ��̈�
		textO[oneNum]->Render(dc,
			dx + space * 8, dy,
			dw * 5, dh * 5,
			0, 0,
			82, 82,
			angle,
			r, g, b, a);
	}

	// �E���� //
	if (!AlignLeft)
	{
		//0�ł��\������
		if (zeroView)
		{
			// ���̈�
			textOku[oku]->Render(dc,
				dx - space * 8, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �疜�̈�
			textThoMan[thoMan]->Render(dc,
				dx - space * 7, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �S���̈�
			textThoMan[hunMan]->Render(dc,
				dx - space * 6, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �\���̈�
			textTenMan[tenMan]->Render(dc,
				dx - space * 5, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// ���̈�
			textMan[man]->Render(dc,
				dx - space * 4, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// ��̈�
			textTho[thousand]->Render(dc,
				dx - space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �S�̈�
			textH[hundred]->Render(dc,
				dx - space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// �\�̈�
			textT[ten]->Render(dc,
				dx - space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 0�̎��\�����Ȃ� //

		else
		{
			// ���̈�
			if (oku != 0)
			{
				textO[oku]->Render(dc,
					dx - space * 8, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �疜�̈�
			if (viewThoMan)
			{
				textThoMan[thoMan]->Render(dc,
					dx - space * 7, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			if (viewHunMan)
			{
				// �S���̈�
				textThoMan[hunMan]->Render(dc,
					dx - space * 6, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �\���̈�
			if (viewTenMan)
			{
				textTenMan[tenMan]->Render(dc,
					dx - space * 5, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// ���̈�
			if (viewMan)
			{
				textMan[man]->Render(dc,
					dx - space * 4, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}


			// ��̈�
			if (viewTho)
			{
				textTho[thousand]->Render(dc,
					dx - space * 3, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �S�̈�
			if (viewHun)
			{
				textH[hundred]->Render(dc,
					dx - space * 2, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// �\�̈�
			if (viewTen) //0�̎��\�����Ȃ�
			{

				textT[ten]->Render(dc,
					dx - space, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}
		}

		// ��̈�
		textO[oneNum]->Render(dc,
			dx, dy,
			dw * 5, dh * 5,
			0, 0,
			82, 82,
			angle,
			r, g, b, a);
	}
}