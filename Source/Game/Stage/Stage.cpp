#include "Stage.h"
#include "StageManager.h"

// �X�e�[�W���폜
void Stage::Destroy()
{
	StageManager::Instance().Remove(this);
}