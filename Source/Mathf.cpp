#include "Mathf.h"
#include <stdlib.h>

// 指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max)
{
	// 0からRAND_MAX（乱数の最大値）までの乱数を生成し、それを0から1までの範囲に変換する
	float random01 = static_cast<float>(rand()) / RAND_MAX;

	// minからmaxまでの範囲に乱数をマップする
	return min + random01 * (max - min);
}