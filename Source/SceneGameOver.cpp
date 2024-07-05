#include "SceneGameOver.h"
#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"

// 初期化
void SceneGameOver::Initialize()
{
}

// 終了化
void SceneGameOver::Finalize()
{
}

// 更新処理
void SceneGameOver::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_START;
	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneLoading* loadingScene = new SceneLoading(new SceneTitle);

		// シーンマネージャーにローディングシーンへの切り替えを指示
		SceneManager::Instance().ChangeScene(loadingScene);
	}
}

// 描画処理
void SceneGameOver::Render()
{
}