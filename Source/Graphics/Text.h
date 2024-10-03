#pragma once
#include "Sprite.h"
#include <memory>

class Text
{
public:
	Text();
	~Text();

	// 描画処理(千の位まで対応(タイムなど用))
	// 【タイムなどの測定として利用する場合】
	// 「measure」を【true】、「oneNum」に変数などを割り当てる。他の位は「0」にしておく。
	// oneNumの対応範囲：【0～9999】
	// 【文字の表示として利用する場合】
	// 「measure」を【false】にする。
	// それぞれの位に数値を割り当てる。
	// 「0～9」まで対応。「0未満」または「10以上」だとエラーが出ます。
	void Render(ID3D11DeviceContext* dc,
		bool measure,					    // 関数を測定に使うか
		bool AlignLeft,                     // 左揃えにするか
		bool zeroViewTho,				    // 0でも表示するか(千の位)
		bool zeroViewHun, bool zeroViewTen, // 0でも表示するか(百の位、十の位)
		int thousand,					    // 数値(千)
		int hundred, int ten, int oneNum,   // 数値(百以降)
		float dx, float dy,				    // 位置
		float dw, float dh,				    // 大きさ
		float angle,					    // 角度
		float space,					    // 文字の間隔
		float r, float g, float b, float a  // 色
	) const;

	// 描画処理(億の位まで対応(スコア用))
	// 【タイムなどの測定として利用する場合】
	//「measure」を【true】、「oneNum」に変数などを割り当てる。他の位は「0」にしておく。
	// oneNumの対応範囲：【0～99999999】
	// 【文字の表示として利用する場合】
	// 「measure」を【false】にする。
	// それぞれの位に数値を割り当てる。
	// 「0～9」まで対応。「0未満」または「10以上」だとエラーが出ます。
	void Render(ID3D11DeviceContext* dc,
		bool measure,									// 関数を測定に使うか
		bool AlignLeft,                                 // 左揃えにするか
		bool zeroView,									// 0の時表示するか
		int oku,										// 数値(億)
		int thoMan, int hunMan, int tenMan, int man,	// 数値(万)
		int thousand, int hundred, int ten, int oneNum,	// 数値(千以降)
		float dx, float dy,								// 位置
		float dw, float dh,								// 大きさ
		float angle,									// 角度
		float space,									// 文字の間隔
		float r, float g, float b, float a				// 色
	) const;

private:
	std::unique_ptr<Sprite> textOku[10];      // 億の位
	std::unique_ptr<Sprite> textThoMan[10]; // 千万の位
	std::unique_ptr<Sprite> textTenMan[10]; // 十万の位
	std::unique_ptr<Sprite> textMan[10];    // 万の位
	std::unique_ptr<Sprite> textTho[10];    // 千の位
	std::unique_ptr<Sprite> textH[10];      // 百の位
	std::unique_ptr<Sprite> textT[10];      // 十の位
	std::unique_ptr<Sprite> textO[10];      // 一の位
};