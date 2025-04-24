#pragma once
#include "Sprite.h"
#include <memory>

class Text
{
public:
	Text();
	~Text();

	/*! ゲッター */

	// 最小値
	int GetMin() const { return limit.min; }

	// 最大値(千の位までの場合)
	int GetMaxTho() const { return limit.maxTho; }

	// 最大値(億の位までの場合)
	int GetMaxOku() const { return limit.maxOku; }

	// 描画処理(千の位まで対応(タイムなど用))
	// 【タイムなどの測定として利用する場合】
	// 「measure」を【true】、「oneNum」に変数などを割り当てる。他の位は「0」にしておく。
	// oneNumの対応範囲：【0～9999】
	// 【文字の表示として利用する場合】
	// 「measure」を【false】にする。
	// それぞれの位に数値を割り当てる。
	// 「0～9」まで対応。「0未満」または「10以上」だとエラーが出ます。
	void RenderThousand(ID3D11DeviceContext* dc,
		bool AlignLeft,                                     // 左揃えにするか
		int oneNum,                                         // 変数を割り当てる
		bool zeroViewTho,				                    // 0でも表示するか(千の位)
		bool zeroViewHun, bool zeroViewTen,                 // 0でも表示するか(百の位、十の位)
		float dx, float dy,				                    // 位置
		float dw = 8, float dh = 8,		                    // 大きさ
		float angle = 0,					                // 角度
		float space = 28,					                // 文字の間隔
		float r = 1, float g = 1, float b = 1, float a = 1, // 色
		int thousand = 0,					                // 数値(千)
		int hundred = 0, int ten = 0                        // 数値(百以降)
	) const;

	// 描画処理(億の位まで対応(スコア用))
	// 【タイムなどの測定として利用する場合】
	//「measure」を【true】、「oneNum」に変数などを割り当てる。他の位は「0」にしておく。
	// oneNumの対応範囲：【0～99999999】
	// 【文字の表示として利用する場合】
	// 「measure」を【false】にする。
	// それぞれの位に数値を割り当てる。
	// 「0～9」まで対応。「0未満」または「10以上」だとエラーが出ます。
	void RenderOku(ID3D11DeviceContext* dc,
		bool AlignLeft,                                              // 左揃えにするか
		int oneNum,                                                  // 変数を割り当てる
		bool zeroView,							                     // 0の時表示するか
		float dx, float dy,								             // 位置
		float dw = 8, float dh = 8,								     // 大きさ
		float angle = 0,									         // 角度
		float space = 28,									         // 文字の間隔
		float r = 1, float g = 1, float b = 1, float a = 1,	         // 色
		int oku = 0,										         // 数値(億)
		int thoMan = 0, int hunMan = 0, int tenMan = 0, int man = 0, // 数値(万)
		int thousand = 0, int hundred = 0, int ten = 0               // 数値(千以降)
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

	struct Limit
	{
		int min    = 0;         // 最小値
		int maxTho = 9999;      // 最大値(千の位までの場合)
		int maxOku = 999999999; // 最大値(億の位までの場合)
	}limit;
};