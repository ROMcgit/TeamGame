#include "Other/Mathf.h"
#include <stdlib.h>

// �w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max)
{
	// 0����RAND_MAX�i�����̍ő�l�j�܂ł̗����𐶐����A�����0����1�܂ł͈̔͂ɕϊ�����
	float random01 = static_cast<float>(rand()) / RAND_MAX;

	// min����max�܂ł͈̔͂ɗ������}�b�v����
	return min + random01 * (max - min);
}