#pragma once
#include "Scene.h"
#include <memory>

// シーンマネージャー
class SceneManager
{
private:
	SceneManager() {};
	~SceneManager() {};

public:
	// 唯一のインスタンス取得
	static SceneManager& Instance()
	{
		static SceneManager instace;
		return instace;
	}

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render();

	// シーンクリア
	void Clear();

	// シーン切り替え
	void ChangeScene(std::unique_ptr<Scene> scene);

private:
	std::unique_ptr<Scene> currentScene;
	std::unique_ptr<Scene> nextScene;
};