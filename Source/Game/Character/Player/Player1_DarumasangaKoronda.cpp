#include <imgui.h>
#include "Player1_DarumasangaKoronda.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Camera/CameraController.h"

static Player1_DarumasangaKoronda* instance = nullptr;

// インスタンス取得
Player1_DarumasangaKoronda& Player1_DarumasangaKoronda::Instance()
{
	return *instance;
}

Player1_DarumasangaKoronda::Player1_DarumasangaKoronda()
{
	// インスタンスポインタ設定
	instance = this;

	// モデル読み込み
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;

	// ヒットエフェクト読み込み
	hitEffect = std::make_unique <Effect>("Data/Effect/Hit.efk");

	// 重力
	gravity = 0.5f;

	// 当たり判定
	radius = 2.3f;
	height = 15.6f;

	// 待機ステートへ遷移
	TransitionWaitState();
}

// デストラクタ
Player1_DarumasangaKoronda::~Player1_DarumasangaKoronda()
{
}

// 更新処理
void Player1_DarumasangaKoronda::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

	// 移動した事が証明されているなら、待機ステートへ遷移する
	if ((gamePad.GetAxisLX() == 0 && gamePad.GetAxisLY() == 0))
		velocity.x = velocity.z = 0;

	// ムービー中なら待機ステートへ遷移
	if (movieScene)
	{
		// 全ての弾を破棄する
		int projectileCount = projectileManager.GetProjectileCount();
		for (int i = 0; i < projectileCount; ++i)
		{
			Projectile* projectile = projectileManager.GetProjectile(i);

			// 弾破棄
			projectile->Destroy();
		}

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

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// モデル更新処理
	model->UpdateTransform(transform);
}

// 描画処理
void Player1_DarumasangaKoronda::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());

	// 弾丸描画処理
	projectileManager.Render(dc, shader);
}

// HPなどのUI描画
void Player1_DarumasangaKoronda::SpriteRender(ID3D11DeviceContext* dc)
{
}

// 移動入力処理
bool Player1_DarumasangaKoronda::InputMove(float elapsedTime)
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
bool Player1_DarumasangaKoronda::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	{
		if (gamePad.GetButtonHeld() & GamePad::BTN_A) //Zキー
		{
			// ジャンプ
			Jump(jumpSpeed);
			jumpCount++;

			// ジャンプ入力した
			return true;

		}
		return false;
	}
}

// 待機ステートへ遷移
void Player1_DarumasangaKoronda::TransitionWaitState()
{
	state = State::Wait;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Wait, true);
}

// 待機ステート更新処理
void Player1_DarumasangaKoronda::UpdateWaitState(float elapsedTime)
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
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		InputJump();
		TransitionJumpState();
	}

	InputMove(elapsedTime);
}

// 移動ステートへ遷移
void Player1_DarumasangaKoronda::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	model->PlayAnimation(Anim_Move, true);
}

// 移動ステート更新処理
void Player1_DarumasangaKoronda::UpdateMoveState(float elapsedTime)
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

	//! ジャンプ処理
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		InputJump();
		TransitionJumpState();
	}

	InputMove(elapsedTime);
}

// ジャンプステートへ遷移
void Player1_DarumasangaKoronda::TransitionJumpState()
{
	state = State::Jump;

	model->PlayAnimation(Anim_Jump, false);
}

// ジャンプステート更新処理
void Player1_DarumasangaKoronda::UpdateJumpState(float elapsedTime)
{
	if (isGround)
		TransitionWaitState();
}

// ダメージステートへ遷移
void Player1_DarumasangaKoronda::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Damage, false);
}

// ダメージステート更新処理
void Player1_DarumasangaKoronda::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionWaitState();
	}
}

// 死亡ステートへ遷移
void Player1_DarumasangaKoronda::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player1_DarumasangaKoronda::UpdateDeathState(float elapsedTimae)
{
}

//デバッグプリミティブ描画
void Player1_DarumasangaKoronda::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// 着地した時に呼ばれる
void Player1_DarumasangaKoronda::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player1_DarumasangaKoronda::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player1_DarumasangaKoronda::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// 当たり判定描画
void Player1_DarumasangaKoronda::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player1_DarumasangaKoronda"))
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