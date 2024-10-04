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
	scale.x = scale.y = scale.z = 0.03f;

	gravity = 0.0f;

	// 幅、高さ設定
	radius = 0.5f;
	height = 1.0f;

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

	// オブジェクト行列を更新
	UpdateTransform();

	// プレイヤーとの当たり判定
	CollisionItemVsPlayer();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
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

// デバッグプリミティブ描画
void Apple::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Item::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	// 縄張り範囲をデバッグ円柱描画
	debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
		DirectX::XMFLOAT4(0, 1, 0, 1));

	// ターゲット位置をデバッグ球描画
	debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	// 索敵範囲をデバッグ円柱描画
	debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	// 攻撃範囲をデバッグ円柱描画
	debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
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