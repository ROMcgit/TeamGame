#include "Apple.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player.h"
#include "Other/Collision.h"

// コンストラクタ
Apple::Apple()
{
	model = std::make_unique<Model>("Data/Model/Item/Apple/Apple.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.007f;

	gravity = 0.0f;

	collisionOffset = { 0, -0.5f, 0 };

	// 幅、高さ設定
	radius = 0.8f;
	height = 1.5f;

	// 待機ステートへ遷移
	TransitionInitState();
}

// デストラクタ
Apple::~Apple()
{
}

// 更新処理
void Apple::Update(float elapsedTime)
{
	position.y = 0.8f;

	// ステート毎の更新処理
	switch (state)
	{
	case State::Init:
		UpdateInitState(elapsedTime);
		break;
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// 当たり判定の位置設定
	CollisionPosSettings();

	// オブジェクト行列を更新
	UpdateTransform();

	// プレイヤーとの当たり判定
	CollisionItemVsPlayer();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);

	// 一定の距離を離れたら破棄する
	Player& player = Player::Instance();

	float vx = player.GetPosition().x - position.x;
	float vz = player.GetPosition().z - position.z;
	dist = vx * vx + vz * vz;
	if (dist > 1200)
		Destroy();
}

// 描画処理
void Apple::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void Apple::SpriteRender(ID3D11DeviceContext* dc)
{

}

// デバッグ用GUI描画
void Apple::DrawDebugGUI()
{
	if (ImGui::TreeNode("Apple"))
	{
		ImGui::InputFloat("Dist", &dist);

		ImGui::InputFloat3("Velocity", &velocity.x);

		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::InputFloat3("Position", &position.x);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::InputFloat3("Scale", &scale.x);
		}
		ImGui::TreePop();
	}
}

// デバッグプリミティブ描画
void Apple::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Item::DrawDebugPrimitive();
}

// プレイヤーとの接触処理
void Apple::CollisionItemVsPlayer()
{
	Player& player = Player::Instance();

	// 衝突処理
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		position,
		radius,
		height,
		outPosition
	))
	{
		// プレイヤーを拡散攻撃できるようにする
		player.SetDiffusionAttacks(true);

		// 破棄する
		Destroy();
	}
}

// 待機ステートへ遷移
void Apple::TransitionInitState()
{
	state = State::Init;

	model->PlayAnimation(0, true);
}

// 待機ステート更新処理
void Apple::UpdateInitState(float elapsedTime)
{
}