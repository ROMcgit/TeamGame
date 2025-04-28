#include "CollisionAttack_Tataki.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"
#include "Game/Character/Enemy/EnemyManager.h"

#include <imgui.h>

// コンストラクタ
CollisionAttack_Tataki::CollisionAttack_Tataki(CollisionAttackManager* manager)
	: CollisionAttack(manager)
{
	collisionType = CollisionType::Sphere;

	radius = 0.0f;

	// マテリアルの色
	materialColor.x = materialColor.y = materialColor.z = 0.3f;

	// 当たり判定の調整
	collisionOffset = { 0.0f, 0.0f, 0.0f };
}

// デストラクタ
CollisionAttack_Tataki::~CollisionAttack_Tataki()
{
}

// 更新処理
void CollisionAttack_Tataki::Update(float elapsedTime)
{
	// 状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// 敵との当たり判定
	CollisionVsEnemy();
}

// 描画処理
void CollisionAttack_Tataki::Render(ID3D11DeviceContext* dc, Shader* shader)
{
}

// デバッグGUI描画
void CollisionAttack_Tataki::DrawDebugGUI()
{
	if (ImGui::TreeNode("Cloud"))
	{
		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform_CollisionAttack"))
		{
#if 1
			// 位置
			ImGui::DragFloat3("Position", &position.x, 0.01f);
			// 当たり判定の位置
			ImGui::DragFloat3("CollisionPos", &collisionPos.x, 0.01f);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::DragFloat3("Angle", &a.x, 0.01f);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);
#endif
		}

		// 当たり判定
		if (ImGui::CollapsingHeader("Collision"))
		{
#if 1
			static const char* items[] = { "Box", "Sphere", "Cylinder" };
			int currentItem = static_cast<int>(collisionType);

			if (ImGui::Combo("Collision Type", &currentItem, items, static_cast<int>(CollisionType::Count))) {
				collisionType = static_cast<CollisionType>(currentItem);
			}

			switch (collisionType)
			{
			case CollisionType::Box:
			{
				// 幅
				ImGui::DragFloat("Width", &width, 0.01f);
				// 高さ
				ImGui::DragFloat("Height", &height, 0.01f);
				// 奥行
				ImGui::DragFloat("Depth", &depth, 0.01f);
			}
			break;
			case CollisionType::Sphere:
			{
				// 半径
				ImGui::DragFloat("Radius", &radius, 0.01f);
			}
			break;
			case CollisionType::Cylinder:
			{
				// 半径
				ImGui::DragFloat("Radius", &radius, 0.01f);
				// 高さ
				ImGui::DragFloat("Height", &height, 0.01f);
			}
			break;
			default:
				break;
			}

			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-------------------------------------------------------------------------------------------------------//
						// デバッグプリミティブ描画の色
			ImGui::ColorEdit3("DebugPrimitiveColor", &debugPrimitiveColor.x);
			// デバッグプリミティブ描画の色の数値
			ImGui::InputFloat3("Color", &debugPrimitiveColor.x);
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // 一行空ける
			ImGui::Separator(); // セクションの間に区切り線を表示
			ImGui::Spacing(); // 一行空ける
			//-------------------------------------------------------------------------------------------------------//
						// 当たり判定をどれくらいずらすか
			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
#endif
		}

		ImGui::TreePop();
	}
}

// 敵との当たり
void CollisionAttack_Tataki::CollisionVsEnemy()
{
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	for(int i = 0; i < enemyCount; i++)
	{
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			collisionPos,
			radius,
			enemy->GetCollisionPos(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition) && radius > 0.0f)
		{
			//! ダメージを与える
			enemy->ApplyDamage(1, 0, 0);
		}
	}
}