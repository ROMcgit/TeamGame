#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Audio/Audio.h"

// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {};
	~SceneTitle() override {};

	// 唯一のインスタンス取得
	static SceneTitle& Instance()
	{
		static SceneTitle instace;
		return instace;
	}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;
public:
	int score              = 0;
	int scorePlus          = 0;
	int scorePlusResetTime = 0;
	int combo              = 0;
	int comboResetTime     = 0;

private:
	Sprite* sprite = nullptr;
	int select = 0;
	bool moziView = false;
	std::unique_ptr<Sprite> mozi[2];
	std::unique_ptr<Sprite> yazirusi;
	std::unique_ptr<AudioSource> sound[2];
	int waitTime = 0;
	std::unique_ptr<AudioSource> bgm;
	std::unique_ptr<Sprite> fadeIn;
	float fadeInView = 1.0f;
	bool sceneOK = false;
};