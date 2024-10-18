#include "Peach.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player.h"
#include "Other/Collision.h"
#include "Audio/SoundEffectManager.h"

// コンストラクタ
Peach::Peach()
{
	model = std::make_unique<Model>("Data/Model/Item/Peach/Peach.mdl");

	getEffect = std::make_unique <Effect>("Data/Effect/Effect/Peach.efk");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.000f;

	SetScaleChange(DirectX::XMFLOAT3(0.012f, 0.012f, 0.012f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.04f));

	gravity = 0.0f;

	collisionOffset = { 0, -1.0f, 0 };

	// 幅、高さ設定
	radius = 1.7f;
	height = 2.0f;

	SoundEffectManager::Instance().LoadSoundEffect("食べる", "Data/Audio/食べる.wav");

	// 待機ステートへ遷移
	TransitionInitState();
}

// デストラクタ
Peach::~Peach()
{
}

// 更新処理
void Peach::Update(float elapsedTime)
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

	// スケール変更更新処理
	UpdateScaleChange(elapsedTime);

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
	if (dist > 3200)
		Destroy();

	// 自然消滅
	lifeTimer -= elapsedTime;
	if (lifeTimer < 0.0f)
	{
		radius = 0.0f;

		SetScaleChange(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0.04f, 0.04f, 0.04f));

		if (scale.x <= 0)
			Destroy();
	}
}

// 描画処理
void Peach::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void Peach::SpriteRender(ID3D11DeviceContext* dc)
{

}

// デバッグ用GUI描画
void Peach::DrawDebugGUI()
{
	if (ImGui::TreeNode("Peach"))
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
void Peach::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Item::DrawDebugPrimitive();
}

// プレイヤーとの接触処理
void Peach::CollisionItemVsPlayer()
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
		// プレイヤーを回復させる
		player.SetExp(1);

		DirectX::XMFLOAT3 e = player.GetPosition();
		e.y += player.GetHeight() * 0.5f;
		getEffect->Play(e, 0.6f);

		SoundEffectManager& sound = SoundEffectManager::Instance();
		sound.StopSoundEffect("食べる");
		sound.PlaySoundEffect("食べる");

		// 破棄する
		Destroy();
	}
}

// 待機ステートへ遷移
void Peach::TransitionInitState()
{
	state = State::Init;

	model->PlayAnimation(0, true);
}

// 待機ステート更新処理
void Peach::UpdateInitState(float elapsedTime)
{
}