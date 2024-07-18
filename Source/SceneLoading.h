#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include <thread>
#include "CameraController.h"

// ローディングシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {};
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
	std::unique_ptr<Sprite> sprite;
	float angle = 0.0f;
	
	Scene* nextScene = nullptr;
	std::unique_ptr<std::thread> thread;

	std::unique_ptr<Sprite> mozi[4];
	float moziView[4];
	bool nextMozi = false;
	int moziNum = 1;

	CameraController* cameraController = nullptr;
};