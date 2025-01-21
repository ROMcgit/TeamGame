#pragma once

#include <vector>
#include "Director.h"
#include <set>

// 演出マネージャー
class DirectorManager
{
public:
	DirectorManager();
	~DirectorManager();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// デバッグGUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 演出登録
	void Register(std::unique_ptr<Director> director);

	// 演出全削除
	void Clear();

	// 演出数取得
	int GetDirectorCount() const { return static_cast<int>(directors.size()); }

	// 演出取得
	Director* GetDirector(int index) { return directors.at(index).get(); }

	// 演出削除
	void Remove(Director* director);

private:
	std::vector<std::unique_ptr<Director>> directors;
	std::set<Director*> removes;
};