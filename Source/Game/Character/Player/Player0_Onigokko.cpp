#include <imgui.h>
#include "Player0_Onigokko.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Scene/G0_Onigokko.h"

static Player0_Onigokko* instance = nullptr;

bool Player0_Onigokko::damage = false;

// インスタンス取得
Player0_Onigokko& Player0_Onigokko::Instance()
{
	return *instance;
}

Player0_Onigokko::Player0_Onigokko()
{
	// インスタンスポインタ設定
	instance = this;

	// ダメージ
	damage = false;

	//! 体力
	hp = 3;

	for(int i = 0; i < 3; i++)
	{
		std::string filePath = "Data/Sprite/0.Onigokko/Heart" + std::to_string(i + 1) + ".png";

		heart[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	// モデル読み込み
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	// 重力
	gravity = 0;

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.6f;
	height = 5.0f;

	// 移動速度
	moveSpeed = 5.0f;

	// 待機ステートへ遷移
	TransitionWaitState();
}

// デストラクタ
Player0_Onigokko::~Player0_Onigokko()
{
}

// 更新処理
void Player0_Onigokko::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

	// 移動していないなら
	if ((gamePad.GetAxisLX() == 0 && gamePad.GetAxisLY() == 0))
		velocity.x = velocity.z = 0;

	if (invincibleTimer > 0)
		opacity = 0.5f;
	else
		opacity = 1.0f;

	// ムービー中なら待機ステートへ遷移
	if (movieScene)
	{
		// ムービー中のアニメーション
		if (!movieAnimation)
		{
			state = State::Wait; // ステートを待機に変更
			model->PlayAnimation(movieAnimNum, movieAnimLoop);
			movieAnimation = true;
		}
	}
	// ムービー中では無い時
	else
		movieAnimation = false; // ムービー中に待機ステートかどうか

	if (!movieScene)
	{
		// ステート毎の処理
		switch (state)
		{
		case State::Wait:
			UpdateWaitState(elapsedTime);
			break;
		case State::Move:
			UpdateMoveState(elapsedTime);
			break;
		case State::Damage:
			UpdateDamageState(elapsedTime);
			break;
		case State::Death:
			UpdateDeathState(elapsedTime);
			break;
		}
	}

	UpdateVelocity(elapsedTime);

	// キャラクター状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// モデル更新処理
	model->UpdateTransform(transform);
}

// 描画処理
void Player0_Onigokko::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity);
}

// HPなどのUI描画
void Player0_Onigokko::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	for(int i = 0; i < hp; i++)
	{
		float textureWidth = static_cast<float>(heart[i]->GetTextureWidth());
		float textureHeight = static_cast<float>(heart[i]->GetTextureHeight());

		//! ハート
		heart[i]->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
}

// 移動入力処理
bool Player0_Onigokko::InputMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 移動処理
	Move3D(moveVec.x, moveVec.z, moveSpeed);

	// 旋回処理
	Turn3D_Player(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return !(moveVec.x == 0.0f && moveVec.z == 0.0f);
}

// 待機ステートへ遷移
void Player0_Onigokko::TransitionWaitState()
{
	state = State::Wait;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void Player0_Onigokko::UpdateWaitState(float elapsedTime)
{
	// 移動入力処理
	// 移動入力されたら移動ステートへ遷移

	const GamePadButton ArrowButton =
		GamePad::BTN_UP |
		GamePad::BTN_LEFT |
		GamePad::BTN_RIGHT |
		GamePad::BTN_DOWN;

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetAxisLX() != 0 || gamePad.GetAxisLY() != 0)
	{
		// 移動ステートへ遷移
		TransitionMoveState();
	}

	InputMove(elapsedTime);
}

// 移動ステートへ遷移
void Player0_Onigokko::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	model->PlayAnimation(Anim_Move, true);
}

// 移動ステート更新処理
void Player0_Onigokko::UpdateMoveState(float elapsedTime)
{
	// 移動入力処理
	const GamePadButton ArrowButton =
		GamePad::BTN_UP |
		GamePad::BTN_LEFT |
		GamePad::BTN_RIGHT |
		GamePad::BTN_DOWN;

	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetAxisLX() == 0 && gamePad.GetAxisLY() == 0)
	{
		// 待機ステートへ遷移
		TransitionWaitState();
	}

	InputMove(elapsedTime);
}

// ダメージステートへ遷移
void Player0_Onigokko::TransitionDamageState()
{
	state = State::Damage;

	stateChangeWaitTimer = 2.2f;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Damage, false);
}

// ダメージステート更新処理
void Player0_Onigokko::UpdateDamageState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (!model->IsPlayAnimation() && stateChangeWaitTimer <= 0.0f)
	{
		position = { 0, 5, 0 };
		TransitionWaitState();
	}
}

// 死亡ステートへ遷移
void Player0_Onigokko::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player0_Onigokko::UpdateDeathState(float elapsedTimae)
{
}

//デバッグプリミティブ描画
void Player0_Onigokko::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifdef _DEBUG
	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// 着地した時に呼ばれる
void Player0_Onigokko::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player0_Onigokko::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player0_Onigokko::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// 当たり判定描画
void Player0_Onigokko::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player0_Onigokko"))
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
			ImGui::DragFloat("Radius", &radius, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
		}

		ImGui::TreePop();
	}
}