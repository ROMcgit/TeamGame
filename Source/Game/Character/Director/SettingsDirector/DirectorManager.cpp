#include "DirectorManager.h"

// コンストラクタ
DirectorManager::DirectorManager()
{
}

// デストラクタ
DirectorManager::~DirectorManager()
{
	Clear();
}

// 更新処理
void DirectorManager::Update(float elapsedTime)
{
	// 更新処理
	for (auto& director : directors)
	{
		director->Update(elapsedTime);
	}

	directors.erase(
		std::remove_if(directors.begin(), directors.end(),
			[this](const std::unique_ptr<Director>& director) {
				return removes.find(director.get()) != removes.end();
			}),
		directors.end()
	);

	removes.clear();
}

// 描画処理
void DirectorManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (auto& director : directors)
	{
		director->Render(dc, shader);
	}
}

// デバッグGUI描画
void DirectorManager::DrawDebugGUI()
{
	for (auto& director : directors)
	{
		director->DrawDebugGUI();
	}
}

void DirectorManager::DrawDebugPrimitive()
{
	for (auto& director : directors)
	{
		director->DrawDebugPrimitive();
	}
}

void DirectorManager::Register(std::unique_ptr<Director> director)
{
	directors.emplace_back(std::move(director));
}

void DirectorManager::Clear()
{
	directors.clear();
}

void DirectorManager::Remove(Director* director)
{
	// 破棄リストに追加
	removes.insert(director);
}
