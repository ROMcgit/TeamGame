#include <imgui.h>
#include "Player5_AsibaWatari.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Bar.h"
#include "Audio/SoundManager.h"

static Player5_AsibaWatari* instance = nullptr;

// インスタンス取得
Player5_AsibaWatari& Player5_AsibaWatari::Instance()
{
	return *instance;
}

Player5_AsibaWatari::Player5_AsibaWatari()
{
	// インスタンスポインタ設定
	instance = this;

	// モデル読み込み
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	// 重力
	gravity = 0.45f;

	// バーを生成
	std::unique_ptr<CollisionAttack_Bar> bar = std::make_unique<CollisionAttack_Bar>(&collisionAttackManager);
	collisionAttackManager.Register(std::move(bar));

	// 当たり判定
	radius = 2.3f;
	height = 15.6f;

	SoundManager& sound = SoundManager::Instance();
	sound.LoadSound("ジャンプ", "Data/Audio/Sound/Jump.wav");

	// 待機ステートへ遷移
	TransitionWaitState();
}

// デストラクタ
Player5_AsibaWatari::~Player5_AsibaWatari()
{
}

// 更新処理
void Player5_AsibaWatari::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

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
		case State::Jump:
			UpdateJumpState(elapsedTime);
			break;
		case State::Damage:
			UpdateDamageState(elapsedTime);
			break;
		case State::Death:
			UpdateDeathState(elapsedTime);
			break;
		}
	}

	// キャラクター状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// 衝突攻撃の更新処理
	collisionAttackManager.Update(elapsedTime);

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// モデル更新処理
	model->UpdateTransform(transform);
}

// 描画処理
void Player5_AsibaWatari::Render(ID3D11DeviceContext* dc, Shader* shader, bool shadowMap)
{
	shader->Draw(dc, model.get());

	if(!shadowMap)
		collisionAttackManager.Render(dc, shader);
}

// HPなどのUI描画
void Player5_AsibaWatari::SpriteRender(ID3D11DeviceContext* dc)
{
}

// 移動入力処理
bool Player5_AsibaWatari::InputMove(float elapsedTime)
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

// ジャンプ入力処理
bool Player5_AsibaWatari::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	{
		if ((gamePad.GetButtonHeld() & GamePad::BTN_A) || (gamePad.GetButtonHeld() & GamePad::BTN_SPACE)) //Zキー
		{
			// ジャンプ入力した
			return true;

		}
		return false;
	}
}

// 待機ステートへ遷移
void Player5_AsibaWatari::TransitionWaitState()
{
	state = State::Wait;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void Player5_AsibaWatari::UpdateWaitState(float elapsedTime)
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

	//! ジャンプ処理
	GamePadButton button = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y | GamePad::BTN_SPACE;
	//! ジャンプ処理
	if (gamePad.GetButtonDown() & button)
	{
		InputJump();
		TransitionJumpState();
	}

	InputMove(elapsedTime);
}

// 移動ステートへ遷移
void Player5_AsibaWatari::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	if (model->GetAnimationNum() != Anim_Move)
		model->PlayAnimation(Anim_Move, true);
}

// 移動ステート更新処理
void Player5_AsibaWatari::UpdateMoveState(float elapsedTime)
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
		velocity.x = velocity.z = 0;

		// 待機ステートへ遷移
		TransitionWaitState();
	}

	GamePadButton button = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y | GamePad::BTN_SPACE;
	//! ジャンプ処理
	if (gamePad.GetButtonDown() & button)
	{
		InputJump();
		TransitionJumpState();
	}

	

	InputMove(elapsedTime);
}

// ジャンプステートへ遷移
void Player5_AsibaWatari::TransitionJumpState()
{
	state = State::Jump;

	SoundManager::Instance().PlaySound("ジャンプ");

	model->PlayAnimation(Anim_Jump, false);
}

// ジャンプステート更新処理
void Player5_AsibaWatari::UpdateJumpState(float elapsedTime)
{
	if (isGround)
		TransitionWaitState();

	// 移動入力処理
	InputMove(elapsedTime);
}

// ダメージステートへ遷移
void Player5_AsibaWatari::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Damage, false);
}

// ダメージステート更新処理
void Player5_AsibaWatari::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionWaitState();
	}
}

// 死亡ステートへ遷移
void Player5_AsibaWatari::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player5_AsibaWatari::UpdateDeathState(float elapsedTimae)
{
}

//デバッグプリミティブ描画
void Player5_AsibaWatari::DrawDebugPrimitive()
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
void Player5_AsibaWatari::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player5_AsibaWatari::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player5_AsibaWatari::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// 当たり判定描画
void Player5_AsibaWatari::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player5_AsibaWatari"))
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