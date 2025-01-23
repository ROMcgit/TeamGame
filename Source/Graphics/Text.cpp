#include "Text.h"
#include <string>
#include <iostream>

Text::Text()
{
	// 画像読み込み
	for (int i = 0; i < 10; i++)
	{
		std::string filePath = "Data/Font/" + std::to_string(i) + ".png";
		// 億の位
		textOku[i]      = std::make_unique<Sprite>(filePath.c_str());
		// 千万の位
		textThoMan[i] = std::make_unique<Sprite>(filePath.c_str());
		// 十万の位
		textTenMan[i] = std::make_unique<Sprite>(filePath.c_str());
		// 万の位
		textMan[i]    = std::make_unique<Sprite>(filePath.c_str());
		// 千の位
		textTho[i]    = std::make_unique<Sprite>(filePath.c_str());
		// 百の位
		textH[i]      = std::make_unique<Sprite>(filePath.c_str());
		// 十の位     
		textT[i]      = std::make_unique<Sprite>(filePath.c_str());
		// 一の位     
		textO[i]      = std::make_unique<Sprite>(filePath.c_str());
	}
}

Text::~Text()
{
}

// 描画処理(百の位まで対応(タイム用))
void Text::RenderThousand(ID3D11DeviceContext* dc,
	bool AlignLeft,                      // 左揃えにするか
	int oneNum,                          // 変数を割り当てる
	bool zeroViewTho,				     // 0でも表示するか(千の位)
	bool zeroViewHun, bool zeroViewTen,  // 0でも表示するか(百の位、十の位)
	float dx, float dy,				     // 位置
	float dw, float dh,		             // 大きさ
	float angle,					     // 角度
	float space,					     // 文字の間隔
	float r, float g, float b, float a,  // 色
	int thousand,					     // 数値(千)
	int hundred, int ten                 // 数値(百以降)
) const
{
	// bool文の宣言 //
	bool viewTho = false;
	bool viewHun = false;
	bool viewTen = false;

	// 表示処理 //
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
	
	// エラーメッセージ //

	// 0より小さいまたは999より大きい時
	if (oneNum < 0 || oneNum > 9999)
	{
		std::cerr << "Error: 一の位に当たる数値が「0」未満または9999より大きいです: " << std::endl;
		throw std::runtime_error("値が許容範囲を超えました！0～9999までの範囲にしてください。");
	}
	// 千の位、または百の位、または十の位が0より小さい、または9より大きい時
	else if (thousand < 0 || hundred < 0 || ten < 0 ||
			thousand > 9 || hundred > 9 || ten > 9)
	{
		std::cerr << "Error: 「百の位」または「十の位」が「0」未満または「10」以上です:" << std::endl;
		throw std::runtime_error("値が許容範囲を超えました！0～9までの範囲にしてください。");
	}

/****************************************************************************************************************************/
	
	// 数値の調整 //
	thousand = oneNum / 1000;  // 千の位を取得
	oneNum %= 1000;            // 残りの値

	hundred = oneNum / 100;   // 百の位を取得
	oneNum %= 100;            // 残りの値

	ten = oneNum / 10;        // 十の位を取得
	oneNum %= 10;             // 一の位を取得
	
/****************************************************************************************************************************/
	
	// 数値の表示 //

	// 左揃え //
	if (AlignLeft)
	{
		// 千の位
		if (zeroViewTho) //0でも表示する
		{
			textH[thousand]->Render(dc,
				dx, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTho) //0の時表示しない
		{
			textH[thousand]->Render(dc,
				dx, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 百の位
		if (zeroViewHun) //0でも表示する
		{
			textH[hundred]->Render(dc,
				dx + space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewHun) //0の時表示しない
		{
			textH[hundred]->Render(dc,
				dx + space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 十の位
		if (zeroViewTen) //0でも表示する
		{
			textT[ten]->Render(dc,
				dx + space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTen) //0の時表示しない
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

	// 右揃え //
	if (!AlignLeft)
	{
		// 千の位
		if (zeroViewTho) //0でも表示する
		{
			textH[thousand]->Render(dc,
				dx - space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTho) //0の時表示しない
		{
			textH[thousand]->Render(dc,
				dx - space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 百の位
		if (zeroViewHun) //0でも表示する
		{
			textH[hundred]->Render(dc,
				dx - space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewHun) //0の時表示しない
		{
			textH[hundred]->Render(dc,
				dx - space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 十の位
		if (zeroViewTen) //0でも表示する
		{
			textT[ten]->Render(dc,
				dx - space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}
		else if (viewTen) //0の時表示しない
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

// 描画処理(億の位まで対応(スコア用))
void Text::RenderOku(ID3D11DeviceContext* dc, 
	bool AlignLeft,                                 // 左揃えにするか
	int oneNum,                                     // 変数を割り当てる
	bool zeroView,									// 0の時表示するか
	float dx, float dy,								// 位置
	float dw, float dh,								// 大きさ
	float angle,									// 角度
	float space,									// 文字の間隔
	float r, float g, float b, float a,				// 色
	int oku,										// 数値(億)
	int thoMan, int hunMan, int tenMan, int man,	// 数値(万)
	int thousand, int hundred, int ten              // 数値(千以降)
) const
{
	// bool型の宣言 //
	bool viewThoMan = false;
	bool viewHunMan = false;
	bool viewTenMan = false;
	bool viewMan    = false;
	bool viewTho    = false;
	bool viewHun    = false;
	bool viewTen    = false;

	// 表示処理 //
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
	
	// エラーメッセージ //

	// 0より小さいまたは1億以上の時
	if (oneNum < 0 || oneNum >= 100000000)
	{
		std::cerr << "Error: 一の位に当たる数値が「0」未満または「1億」以上です: " << std::endl;
		throw std::runtime_error("値が許容範囲を超えました！0～1億未満までの範囲にしてください。");
	}
	// 数値が範囲外の時
	else if (oku < 0 || thoMan < 0 || hunMan < 0 || tenMan < 0 || man < 0 || thousand < 0 || hundred < 0 || ten < 0 ||
			oku > 9 || thoMan > 9 || hunMan > 9 || tenMan > 9 || man > 9 || thousand > 9 || hundred > 9 || ten > 9)
	{
		std::cerr << "Error: 割り当てられた数値が「0」未満または「10」以上です:" << std::endl;
		throw std::runtime_error("値が許容範囲を超えました！0～9までの範囲にしてください。");
	}

/****************************************************************************************************************************/

	// 数値の調整 //
	// 億の位
	oku = oneNum / 100000000;
	oneNum %= 100000000;

	// 千万の位
	thoMan = oneNum / 10000000;
	oneNum %= 10000000;

	// 百万の位
	hunMan = oneNum / 1000000;
	oneNum %= 1000000;

	// 十万の位
	tenMan = oneNum / 100000;
	oneNum %= 100000;

	// 万の位
	man = oneNum / 10000;
	oneNum %= 10000;

	// 千の位
	thousand = oneNum / 1000;
	oneNum %= 1000;

	// 百の位
	hundred = oneNum / 100;
	oneNum %= 100;

	// 十の位
	ten = oneNum / 10;
	oneNum %= 10;
	
/****************************************************************************************************************************/
	
	// 数値の表示 //

	// 左揃え //
	if (AlignLeft)
	{
		//0でも表示する
		if (zeroView)
		{
			// 億の位
			textOku[oku]->Render(dc,
				dx, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 千万の位
			textThoMan[thoMan]->Render(dc,
				dx + space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 百万の位
			textThoMan[hunMan]->Render(dc,
				dx + space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 十万の位
			textTenMan[tenMan]->Render(dc,
				dx + space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 万の位
			textMan[man]->Render(dc,
				dx + space * 4, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 千の位
			textTho[thousand]->Render(dc,
				dx + space * 5, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 百の位
			textH[hundred]->Render(dc,
				dx + space * 6, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 十の位
			textT[ten]->Render(dc,
				dx + space * 7, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 0の時表示しない //

		else
		{
			// 億の位
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

			// 千万の位
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
				// 百万の位
				textThoMan[hunMan]->Render(dc,
					dx + space * 2, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// 十万の位
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

			// 万の位
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


			// 千の位
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

			// 百の位
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

			// 十の位
			if (viewTen) //0の時表示しない
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

		// 一の位
		textO[oneNum]->Render(dc,
			dx + space * 8, dy,
			dw * 5, dh * 5,
			0, 0,
			82, 82,
			angle,
			r, g, b, a);
	}

	// 右揃え //
	if (!AlignLeft)
	{
		//0でも表示する
		if (zeroView)
		{
			// 億の位
			textOku[oku]->Render(dc,
				dx - space * 8, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 千万の位
			textThoMan[thoMan]->Render(dc,
				dx - space * 7, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 百万の位
			textThoMan[hunMan]->Render(dc,
				dx - space * 6, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 十万の位
			textTenMan[tenMan]->Render(dc,
				dx - space * 5, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 万の位
			textMan[man]->Render(dc,
				dx - space * 4, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 千の位
			textTho[thousand]->Render(dc,
				dx - space * 3, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 百の位
			textH[hundred]->Render(dc,
				dx - space * 2, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);

			// 十の位
			textT[ten]->Render(dc,
				dx - space, dy,
				dw * 5, dh * 5,
				0, 0,
				82, 82,
				angle,
				r, g, b, a);
		}

		// 0の時表示しない //

		else
		{
			// 億の位
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

			// 千万の位
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
				// 百万の位
				textThoMan[hunMan]->Render(dc,
					dx - space * 6, dy,
					dw * 5, dh * 5,
					0, 0,
					82, 82,
					angle,
					r, g, b, a);
			}

			// 十万の位
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

			// 万の位
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


			// 千の位
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

			// 百の位
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

			// 十の位
			if (viewTen) //0の時表示しない
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

		// 一の位
		textO[oneNum]->Render(dc,
			dx, dy,
			dw * 5, dh * 5,
			0, 0,
			82, 82,
			angle,
			r, g, b, a);
	}
}