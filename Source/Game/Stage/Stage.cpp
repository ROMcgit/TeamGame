#include "Stage.h"
#include "StageManager.h"

// ステージを削除
void Stage::Destroy()
{
	StageManager::Instance().Remove(this);
}