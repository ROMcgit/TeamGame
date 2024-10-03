#include "Game/Character/Installation/InstallationManager.h"
#include "Other/Collision.h"

InstallationManager::InstallationManager()
{
}

InstallationManager::~InstallationManager()
{
	Clear();
}

// 更新処理
void InstallationManager::Update(float elapsedTime)
{
	for (auto& installations : installations)
	{
		installations->Update(elapsedTime);
	}

	for (Installation* installation : removes)
	{
		auto it = std::remove_if(installations.begin(), installations.end(),
			[installation](const std::unique_ptr<Installation>& ptr) { return ptr.get() == installation; });

		if (it != installations.end())
		{
			installations.erase(it, installations.end());
		}
	}

	removes.clear();
}

// 描画処理
void InstallationManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (auto& installations : installations)
	{
		installations->Render(context, shader);
	}
}

// 2Dスプライトの描画処理
void InstallationManager::SpriteRender(ID3D11DeviceContext* dc)
{
	for (auto& installations : installations)
	{
		installations->SpriteRender(dc);
	}
}

// HP表示
void InstallationManager::RenderInstallationGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	for (auto& installations : installations)
	{
		installations->RenderInstallationGauge(dc, view, projection);
	}
}

// デバッグプリミティブ描画
void InstallationManager::DrawDebugPrimitive()
{
	for (auto& installations : installations)
	{
		installations->DrawDebugPrimitive();
	}

}

// エネミー登録
void InstallationManager::Register(std::unique_ptr<Installation> installation)
{
	installations.emplace_back(std::move(installation));
}

// エネミー全削除
void InstallationManager::Clear()
{
	installations.clear();
}

void InstallationManager::Remove(Installation* installations)
{
	// 破棄リストに追加
	removes.push_back(installations);
}

void InstallationManager::CollisionInstallationVsEnemies()
{
	InstallationManager& installationsManager = InstallationManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int installationsCount = installationsManager.GetInstallationCount();
	for (int i = 0; i < installationsCount; ++i)
	{
		std::unique_ptr<Installation>& installations1 = installationsManager.GetInstallation(i);

		for (int j = i + 1; j < installationsCount; ++j)
		{
			std::unique_ptr<Installation>& installations2 = installationsManager.GetInstallation(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;

			if (Collision::IntersectCylinderVsCylinder(
				installations1->GetPosition(),
				installations1->GetRadius(),
				installations1->GetHeight(),
				installations2->GetPosition(),
				installations2->GetRadius(),
				installations2->GetHeight(),
				outPosition
			))
			{
				// 押し出しの後の位置設定
				installations2->SetPosition(outPosition);
			}
		}
	}
}
