#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include <thread>
#include "Graphics/Fade.h"

// ローディングシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(std::unique_ptr<Scene> nextScene) : nextScene(std::move(nextScene)) {};
	~SceneLoading() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Fade> fade;
	bool setFade = false;

	std::unique_ptr<Sprite> loading[4];
	int loadingNum = 0;
	float loadingTime = 0.0f;

	std::unique_ptr<Scene> nextScene;
	std::unique_ptr<std::thread> thread;
};