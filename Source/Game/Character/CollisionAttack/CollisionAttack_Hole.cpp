#include "CollisionAttack_Hole.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"

#include <imgui.h>

// コンストラクタ
CollisionAttack_Hole::CollisionAttack_Hole(CollisionAttackManager* manager)
	: CollisionAttack(manager)
{
	model = std::make_unique<Model>("Data/Model/1.Character/1.CuppyHunters/1.Boss/0.FrolicOcean/1.Beamman/Projectile/BeamWaveBullet.mdl");

	// 角度
	angle.x = DirectX::XMConvertToRadians(180);

	// 大きさ
	scale.x = scale.y = scale.z = 0.01f;

	// 不透明度
	opacity = 0;
	// 不透明度を変える
	SetOpacityChange(0.9f, 0.5f);

	// 当たり判定
	radius = 1.2f;

	// 当たり判定の調整
	collisionOffset = { 0.0f, 0.0f, 0.0f };
}

// デストラクタ
CollisionAttack_Hole::~CollisionAttack_Hole()
{
}

// 更新処理
void CollisionAttack_Hole::Update(float elapsedTime)
{
	// 状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void CollisionAttack_Hole::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity);
}

// デバッグGUI描画
void CollisionAttack_Hole::DrawDebugGUI()
{
	if (ImGui::TreeNode("Hole"))
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