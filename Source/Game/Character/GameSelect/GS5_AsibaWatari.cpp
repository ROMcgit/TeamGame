#include "GS5_AsibaWatari.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_OniGokko.h"
#include "Other/Collision.h"
#include "Game/Camera/CameraController.h"
#include "Game/Scene/SceneGameSelect.h"
#include "Input/Input.h"

// コンストラクタ
GS5_AsibaWatari::GS5_AsibaWatari()
{
	model = std::make_unique<Model>("Data/Model/GameSelect/Onigokko.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.05f;

	angle.y = DirectX::XMConvertToRadians(180);

	collisionType = CollisionType::Box;
	// 幅、高さ設定
	width = 22.55f;
	height = 7.0f;
	depth = 2.5f;

	collisionOffset.y = 0.0f;

	gravity = 0.0f;
}

// デストラクタ
GS5_AsibaWatari::~GS5_AsibaWatari()
{
	//delete model;
}

// 更新処理
void GS5_AsibaWatari::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	}

	// キャラクターの状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// プレイヤーとの当たり判定
	CollisionVsPlayer();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void GS5_AsibaWatari::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// HPなどの描画
void GS5_AsibaWatari::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void GS5_AsibaWatari::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	GameSelect::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
}

// デバッグGUI
void GS5_AsibaWatari::DrawDebugGUI()
{
	if (ImGui::TreeNode("GS5_AsibaWatari"))
	{
		ImGui::InputFloat3("Velocity", &velocity.x);

		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::DragFloat3("Position", &position.x, 0.01f);
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
		}

		if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("Width", &width, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
			ImGui::DragFloat("Depth", &depth, 0.01f);
			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
		}

		ImGui::TreePop();
	}
}

// 縄張り設定
void GS5_AsibaWatari::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// ターゲット位置をランダム設定
void GS5_AsibaWatari::SetRandomTargetPosition()
{
	// 縄張り範囲内でランダムな位置を生成
	float randomX = Mathf::RandomRange(territoryOrigin.x - territoryRange, territoryOrigin.x + territoryRange);
	float randomZ = Mathf::RandomRange(territoryOrigin.z - territoryRange, territoryOrigin.z + territoryRange);

	// Y座標はスライムの高さに設定するか、固定の地面の高さに設定する
	float randomY = territoryOrigin.y; // または固定値、例: 0.0f

	// ターゲット位置に設定
	targetPosition.x = randomX;
	targetPosition.y = randomY;
	targetPosition.z = randomZ;
}

// 目標地点へ移動
void GS5_AsibaWatari::MoveToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move3D(vx, vz, moveSpeed * speedRate);
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// 待機ステートへ遷移
void GS5_AsibaWatari::TransitionWaitState()
{
	state = State::Wait;

	// タイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void GS5_AsibaWatari::UpdateWaitState(float elapsedTime)
{
}

// プレイヤーとの接触処理
void GS5_AsibaWatari::CollisionVsPlayer()
{
	Player0_Onigokko& player = Player0_Onigokko::Instance();

	GamePad& gamePad = Input::Instance().GetGamePad();
	GamePadButton button = GamePad::BTN_A | GamePad::BTN_B;

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectBoxVsCylinder(
		collisionPos,
		width,
		height,
		depth,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		//! 位置を設定
		player.SetPosition(outPosition);

		if (gamePad.GetButtonDown() & button)
		{
			//! シーンを切り替える
			SceneGameSelect::gameSelect = SceneGameSelect::GameSelect::Asibawatari;
			SceneGameSelect::sceneChange = true;
			player.SetPosition(outPosition);
		}
	}
}