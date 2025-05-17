#include "CollisionAttack_Cloud.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Audio/SoundManager.h"

#include <imgui.h>

// コンストラクタ
CollisionAttack_Cloud::CollisionAttack_Cloud(CollisionAttackManager* manager)
	: CollisionAttack(manager)
{
	model = std::make_unique<Model>("Data/Model/3.SoratobuHusenWari/Cloud/Cloud.mdl");

	// 角度
	angle.x = DirectX::XMConvertToRadians(180);

	// 大きさ
	scale.x = scale.y = scale.z = 0.005f;
	scaleReset = scale;

	collisionType = CollisionType::Box;

	width  = 3.85f;
	height = 2.3f;
	depth  = 1.4f;

	// マテリアルの色
	materialColor.x = materialColor.y = materialColor.z = 0.3f;

	// 当たり判定の調整
	collisionOffset = { 0.0f, 0.0f, 0.0f };
}

// デストラクタ
CollisionAttack_Cloud::~CollisionAttack_Cloud()
{
}

// 更新処理
void CollisionAttack_Cloud::Update(float elapsedTime)
{
	if (G3_SoratobuHusenWari::gameTimer < 30.0f)
		velocity.z = -8.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
		velocity.z = -12.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
		velocity.z = -16.0f;
	else
		velocity.z = -20.0f;

	//position.z = Player3_SoratobuHusenWari::Instance().GetPosition().z + 3;
	if (Player3_SoratobuHusenWari::Instance().GetPosition().z - 0.5f > position.z)
		SetOpacityChange(0.5f, 0.3f);

	if (position.z < -18.0f)
		Destroy();

	// 状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// 状態更新処理
	UpdateState();

	// プレイヤーとの接触処理
	CollisionVsPlayer();

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void CollisionAttack_Cloud::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// デバッグGUI描画
void CollisionAttack_Cloud::DrawDebugGUI()
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

// エミシッブの更新処理
void CollisionAttack_Cloud::UpdateState()
{
	float scale = this->scale.x == scaleReset.x ? scaleReset.x + 0.002f : scaleReset.x;
	//! 大きさを変更する
	SetScaleAllChange(DirectX::XMFLOAT3(scale, scale, scale), 0.2f);

	float emissive = emissiveStrength == 0.0f ? 0.7f : 0.0f;
	//! エミシッブの強さを変更
	SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 1, 1), emissive, 0.2f);
}

// プレイヤーとの接触
void CollisionAttack_Cloud::CollisionVsPlayer()
{
	Player3_SoratobuHusenWari& player = Player3_SoratobuHusenWari::Instance();

	//! プレイヤーと接触したら
	if (CollisionVsPlayer3() && !player.isDamage)
	{
		player.isDamage = true;
	}
}