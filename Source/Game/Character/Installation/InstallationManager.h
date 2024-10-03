#pragma once

#include <vector>
#include"Installation.h"
#include <set>
#include <stdexcept>

// エネミーマネージャー
class InstallationManager
{
private:
	InstallationManager();
	~InstallationManager();

public:
	// 唯一のインスタンス取得
	static InstallationManager& Instance()
	{
		static InstallationManager instance;
		return instance;
	}

	// エネミー数取得
	int GetInstallationCount() const { return static_cast<int>(installations.size()); }

	// エネミー取得
	std::unique_ptr<Installation>& GetInstallation(int index) {
		if (index < 0 || index >= static_cast<int>(installations.size()))
		{
			throw std::out_of_range("Installation index is out of range");
		}
		return installations[index];
	}

	// 更新処理
	void Update(float elapsedTime);

	// エネミーの登録
	void Register(std::unique_ptr<Installation> enemy);

	// エネミー削除
	void Remove(Installation* enemy);

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// 2Dスプライトの描画処理
	void SpriteRender(ID3D11DeviceContext* dc);

	// HP表示
	void RenderInstallationGauge(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection);

	// エネミー全削除
	void Clear();

private:
	// エネミー同士の衝突処理
	void CollisionInstallationVsEnemies();

	std::vector<std::unique_ptr<Installation>> installations;
	std::vector<Installation*> removes;
};