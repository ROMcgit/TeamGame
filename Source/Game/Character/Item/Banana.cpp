#include "Banana.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player.h"
#include "Other/Collision.h"
#include "Audio/SoundEffectManager.h"

// コンストラクタ
Banana::Banana()
{
	model = std::make_unique<Model>("Data/Model/Item/Banana/Banana.mdl");

	SoundEffectManager::Instance().LoadSoundEffect("バナナ", "Data/Audio/バナナ.wav");

	getEffect = std::make_unique <Effect>("Data/Effect/Effect/Banana.efk");

	Player& player = Player::Instance();

	if (player.GetBananaNum() < 3)
	{
		position.x = player.GetPosition().x + ((rand() % 100 + 200) * (rand() % 2 == -1 ? -1 : 1));
		position.z = player.GetPosition().x + ((rand() % 100 + 200) * (rand() % 2 == -1 ? -1 : 1));
	}
	else if(player.GetBananaNum() >= 3 && player.GetBananaNum() < 5)
	{
		position.x = player.GetPosition().x + ((rand() % 100 + 400) * (rand() % 2 == -1 ? -1 : 1));
		position.z = player.GetPosition().x + ((rand() % 100 + 400) * (rand() % 2 == -1 ? -1 : 1));
	}
	else if (player.GetBananaNum() >= 5)
	{
		position.x = player.GetPosition().x + ((rand() % 100 + 700) * (rand() % 2 == -1 ? -1 : 1));
		position.z = player.GetPosition().x + ((rand() % 100 + 700) * (rand() % 2 == -1 ? -1 : 1));
	}

	/// X座標制限
	if (position.x > 1000.0f)
		position.x = 800.0f;
	else if (position.x < -1000.0f)
		position.x = -800.0f;

	// Z座標制限
	if (position.z > 1000.0f)
		position.z = 800.0f;
	else if (position.z < -1000.0f)
		position.z = -800.0f;

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.015f;

	gravity = 0.0f;

	collisionOffset = { 0, -0.0f, 0 };

	// 幅、高さ設定
	radius = 1.2f;
	height = 1.5f;

	// 待機ステートへ遷移
	TransitionInitState();
}

// デストラクタ
Banana::~Banana()
{
}

// 更新処理
void Banana::Update(float elapsedTime)
{
	position.y = 0.5f;

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

}

// 描画処理
void Banana::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void Banana::SpriteRender(ID3D11DeviceContext* dc)
{

}

// デバッグ用GUI描画
void Banana::DrawDebugGUI()
{
	if (ImGui::TreeNode("Banana"))
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
void Banana::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	ImportantItem::DrawDebugPrimitive();
}

// プレイヤーとの接触処理
void Banana::CollisionItemVsPlayer()
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
		if (player.GetInvincibleTimer() <= 0.0f)
		{
			SoundEffectManager::Instance().PlaySoundEffect("バナナ");

			// プレイヤーを拡散攻撃できるようにする
			player.SetBananaNum(1);

			DirectX::XMFLOAT3 e = player.GetPosition();
			e.y += player.GetHeight() * 0.5f;
			getEffect->Play(e, 0.6f);

			// 破棄する
			Destroy();
		}
	}
}

// 待機ステートへ遷移
void Banana::TransitionInitState()
{
	state = State::Init;

	model->PlayAnimation(0, true);
}

// 待機ステート更新処理
void Banana::UpdateInitState(float elapsedTime)
{
	Player& player = Player::Instance();

	float vx = player.GetPosition().x - position.x;
	float vz = player.GetPosition().z - position.z;
	dist = vx * vx + vz * vz;

	dist = (int)(dist + 0.5f);
}