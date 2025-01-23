#include "Togemaru.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"

// コンストラクタ
Togemaru::Togemaru()
{
	model = std::make_unique<Model>("Data/Model/Obstacles/Obstacles.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// 幅、高さ設定
	radius = 0.5f;

	// 移動ステートへ遷移
	TransitionMoveState();
}

// デストラクタ
Togemaru::~Togemaru()
{
	//delete model;
}

// 更新処理
void Togemaru::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
		// 移動
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	}

	// プレイヤーとの当たり判定
	CollisionVsPlayer();

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// キャラクターの状態更新処理
	UpdateCharacterState(elapsedTime);

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void Togemaru::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void Togemaru::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// デバッグプリミティブ描画
void Togemaru::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Obstacle::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
}

// プレイヤーとの当たり判定
void Togemaru::CollisionVsPlayer()
{
	Player0_Onigokko& player = Player0_Onigokko::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectSphereVsCylinder(
		position,
		radius,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		
	}
}

// 移動ステートへ遷移
void Togemaru::TransitionMoveState()
{
	state = State::Move;
}

// 移動ステート更新処理
void Togemaru::UpdateMoveState(float elapsedTime)
{
}