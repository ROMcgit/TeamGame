#include "SceneClear.h"
#include "Camera.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneGameOver.h"
#include "SceneManager.h"
#include "SceneTitle.h"

// 初期化
void SceneClear::Initialize()
{
}

// 終了化
void SceneClear::Finalize()
{
}

// 更新処理
void SceneClear::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B;
	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneLoading* loadingScene = new SceneLoading(new SceneTitle);

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(loadingScene);
	}
}

// 描画処理
void SceneClear::Render()
{
}