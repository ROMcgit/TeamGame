#include <imgui.h>
#include "Player2_Sundome.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Camera/CameraController.h"
#include "Game/Scene/G2_Sundome.h"
#include "Game/Scene/SceneManager.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/G2_Sundome_Result.h"

static Player2_Sundome* instance = nullptr;

// インスタンス取得
Player2_Sundome& Player2_Sundome::Instance()
{
	return *instance;
}

Player2_Sundome::Player2_Sundome()
{
	// インスタンスポインタ設定
	instance = this;

	// モデル読み込み
	models[0] = std::make_unique <Model>("Data/Model/2.Sundome/Suika/Suika.mdl");
	models[1] = std::make_unique <Model>("Data/Model/2.Sundome/TogeBall/TogeBall.mdl");
	models[2] = std::make_unique <Model>("Data/Model/2.Sundome/Person/Person.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.03f;
	scaleReset = scale;

	debugPrimitiveColor = { 1, 0, 0 };

	collisionType = CollisionType::Sphere;

	radius = 3.03f;

	gravity = 0.5f;

	// フェード
	fade = std::make_unique<Fade>();

	// 加速度の数値
	velocityText = std::make_unique<Text>();
	velocityTextSyosuten = std::make_unique<Text>();

	// 加速度の画像
	velocitySprite = std::make_unique<Sprite>();

	// 待機ステートへ遷移
	TransitionMovieWaitState();
}

// デストラクタ
Player2_Sundome::~Player2_Sundome()
{
}

// 更新処理
void Player2_Sundome::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	fade->Update(elapsedTime);

	// ステート毎の処理
	switch (state)
	{
	case State::MovieWait:
		UpdateMovieWaitState(elapsedTime);
		break;
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	case State::Return:
		UpdateReturnState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// キャラクター状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayer2_SundomeVsEnemies();

	// モデル更新処理
	for(int i = 0; i < 3; i++)
		models[i]->UpdateTransform(transform);
}

// 描画処理
void Player2_Sundome::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, models[round - 1].get());

	// 弾丸描画処理
	projectileManager.Render(dc, shader);
}

// HPなどのUI描画
void Player2_Sundome::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	float textureWidth = static_cast<float>(velocitySprite->GetTextureWidth());
	float textureHeight = static_cast<float>(velocitySprite->GetTextureHeight());

	float velocityHeight = (setVelocityX / velocityLimit.max) * spriteScale.y;

	if(!G2_Sundome::movieScene)
	{
		//! 加速度の描画
		if(state == State::Wait)
		{
			velocitySprite->Render(dc,
				spritePos.x, spritePos.y,
				spriteScale.x, velocityHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 0, 0, 1);
		}

		float velocityX = abs((int)velocity.x);
		double velocityXSyosuten = abs(velocity.x) - velocityX;
		velocityXSyosuten = (int)(velocityXSyosuten * 10);

		//! 加速度の数値の描画
		velocityText->RenderOku(dc, false, velocityX, false,
			velocityTextPos.x, velocityTextPos.y);

		//! 加速度の数値(小数点)の描画
		velocityTextSyosuten->RenderOku(dc, false, velocityXSyosuten, false,
			velocityTextPos.x + 50.0f, velocityTextPos.y);
	}

	//! フェード
	fade->Render(dc, graphics);
}

// 移動入力処理
bool Player2_Sundome::InputMove(float elapsedTime)
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

// ムービー待機ステートへ遷移
void Player2_Sundome::TransitionMovieWaitState()
{
	state = State::MovieWait;

	position = { 215.0f, 70.0f, -2.0f };

	scale = scaleReset;

	angle = { 0, 0, 0 };

	switch (round)
	{
	case 1: break;
	case 2:
	{
		angle.x = DirectX::XMConvertToRadians(-90);
		angle.y = DirectX::XMConvertToRadians(90);
	}
		break;
	case 3:
	{
		angle.x = DirectX::XMConvertToRadians(-90);
		angle.y = DirectX::XMConvertToRadians(90);

		scale.x = scale.y = scale.z = 0.05f;
	}
		break;
	default:
		break;
	}
}

// ムービー待機ステート更新処理
void Player2_Sundome::UpdateMovieWaitState(float elapsedTime)
{
	if (!G2_Sundome::movieScene)
		TransitionWaitState();
}

// 待機ステートへ遷移
void Player2_Sundome::TransitionWaitState()
{
	state = State::Wait;
}

// 待機ステート更新処理
void Player2_Sundome::UpdateWaitState(float elapsedTime)
{
	setVelocityX += (30 * elapsedTime) * (velocityDown ? -1 : 1);

	if (setVelocityX < velocityLimit.min || setVelocityX > velocityLimit.max)
	{
		setVelocityX = std::clamp(setVelocityX, velocityLimit.min, velocityLimit.max);
	
		//! 加速度を反転
		velocityDown = !velocityDown;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	GamePadButton button =
	GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	if (gamePad.GetButtonDown() & button)
	{
		//! 移動ステートに遷移
		TransitionMoveState();
	}
}

// 移動ステートへ遷移
void Player2_Sundome::TransitionMoveState()
{
	state = State::Move;

	//! ブレーキ
	switch (round)
	{
	case 1: brake = 8.0f; break;
	case 2: brake = 25.0f; break;
	case 3: brake = 30.0f; break;
	default:
		break;
	}

	stateChangeWaitTimer = 1.0f;
}

// 移動ステート更新処理
void Player2_Sundome::UpdateMoveState(float elapsedTime)
{
	if (round == 3)
		brake = rand() % 30 + 13.0f;

	velocity.x = setVelocityX * -1;

	float rotationSpeed = setVelocityX / velocityLimit.max * 600;

	switch (round)
	{
	case 1:
	{
		angle.z += DirectX::XMConvertToRadians(rotationSpeed) * elapsedTime;
	}
		break;
	case 2:
	{
		angle.x -= DirectX::XMConvertToRadians(rotationSpeed) * elapsedTime;
	}
		break;
	case 3:
	{
		angle.x -= DirectX::XMConvertToRadians(rotationSpeed) * elapsedTime;
	}
		break;
	default:
		break;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	float velocityX = abs((int)velocity.x);
	double velocityXSyosuten = abs(velocity.x) - velocityX;
	velocityXSyosuten = (int)(velocityXSyosuten * 10);

	GamePadButton button =
		GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	if ((gamePad.GetButtonHeld() & button && position.x < 205.0f
		&& ((setVelocityX > 0.0f && velocityXSyosuten >= 0.0f) || (setVelocityX <= 0.0f && velocityXSyosuten > 0.0f)) && isGround)
		|| setVelocityX <= 13.0f && ((setVelocityX > 0.0f && velocityXSyosuten >= 0.0f) || (setVelocityX <= 0.0f && velocityXSyosuten > 0.0f)))
		setVelocityX -= brake * elapsedTime;
	else if ((int)setVelocityX <= 0.0f && velocityXSyosuten <= 0.0f)
		setVelocityX = 0.0f;

	//! ラウンドが2で着地しているなら
	if (round == 2 && isGround && boundTimer > 0.7f && setVelocityX > 0.0f)
	{
		boundTimer = 0.0f;
		velocity.y = 5.0f;
	}
	else
		boundTimer += elapsedTime;

	//! 位置が30より小さいなら
	if (position.y < 30.0f)
	{
		//! 死亡ステートへ遷移
		TransitionDeathState();
	}
	else if (setVelocityX == 0)
	{
		stateChangeWaitTimer -= elapsedTime;

		if(stateChangeWaitTimer <= 0.0f)
		{
			TransitionReturnState();
		}
	}
}

// 戻るステートへ遷移
void Player2_Sundome::TransitionReturnState()
{
	state = State::Return;

	setVelocityX = 0.0f;

	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		0.0f, 1.0f,
		1.0f, 0.2f);

	stateChangeWaitTimer = 0.5f;
}

// 戻るステート更新処理
void Player2_Sundome::UpdateReturnState(float elapsedTime)
{
	if (!fade->GetFade())
	{
		stateChangeWaitTimer -= elapsedTime;

		if (stateChangeWaitTimer <= 0.0f)
		{
			G2_Sundome::movieScene = true;

			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				1.0f, 0.0f,
				1.0f, 0.6f);

			if (round < 3)
				round++;
			else
			{
				std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G2_Sundome_Result>());

				// シーンマネージャーにローディングシーンへの切り替えを指示
				SceneManager::Instance().ChangeScene(std::move(loadingScene));
			}

			//! ムービー待機ステートへ遷移
			TransitionMovieWaitState();
		}
	}
}

// ダメージステートへ遷移
void Player2_Sundome::TransitionDamageState()
{
	state = State::Damage;

	setVelocityX = 0.0f;
}

// ダメージステート更新処理
void Player2_Sundome::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionWaitState();
	}
}

// 死亡ステートへ遷移
void Player2_Sundome::TransitionDeathState()
{
	state = State::Death;

	setVelocityX = 0.0f;

	DirectX::XMFLOAT3 colorF = Camera::postEffect.colorFilter;
	colorF.x += 2.0f;
	
	// カラーフィルターを変更する
	SetColorFilterChange(DirectX::XMFLOAT3(colorF), 0.5f);

	//! フェードを設定
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		0.0f, 1.0f,
		1.0f, 0.5f);

	stateChangeWaitTimer = 1.0f;
}

// 死亡ステート更新処理
void Player2_Sundome::UpdateDeathState(float elapsedTimae)
{
	float scale = 0.0f;
	SetScaleAllChange(DirectX::XMFLOAT3(scale, scale, scale), 1.5f);

	if (!scaleChange.all && !fade->GetFade())
	{
		stateChangeWaitTimer -= elapsedTimae;

		if (stateChangeWaitTimer <= 0.0f)
		{
			G2_Sundome::movieScene = true;

			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				1.0f, 0.0f,
				1.0f, 0.6f);

			DirectX::XMFLOAT3 colorF = Camera::colorFilterReset;
			// カラーフィルターを変更する
			SetColorFilterChange(DirectX::XMFLOAT3(colorF), 0.5f);

			if (round < 3)
				round++;
			else
			{
				std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G2_Sundome_Result>());

				// シーンマネージャーにローディングシーンへの切り替えを指示
				SceneManager::Instance().ChangeScene(std::move(loadingScene));
			}

			//! ムービー待機ステートへ遷移
			TransitionMovieWaitState();
		}
	}
}

// プレイヤーとエネミーとの衝突処理
void Player2_Sundome::CollisionPlayer2_SundomeVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		//if (Collision::IntersectSphereVsSphere(
		//	Player2_Sundome::GetPosition(),
		//	Player2_Sundome::GetRadius(),
		//	enemy->GetPosition(),
		//	enemy->GetRadius(),
		//	outPosition
		//))
		//{
		//	// 押し出しの後の位置設定
		//	enemy->SetPosition(outPosition);
		//}


		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
			//// プレイヤーが敵の上にいるかを判定する
			//float diff = Player2_Sundome::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player2_Sundome::Jump(10);
			//	// 小ジャンプさせるためにY方向の速度を設定する
			//}

			//// 押し出しの後の位置設定
			//enemy->SetPosition(outPosition);

			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			// 上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				// 小ジャンプする
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				// 押し出し後の位置設定
				enemy->SetPosition(outPosition);
			}

		}

	}
}

//デバッグプリミティブ描画
void Player2_Sundome::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawSphere(collisionPos, radius, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// 着地した時に呼ばれる
void Player2_Sundome::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player2_Sundome::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player2_Sundome::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// 当たり判定描画
void Player2_Sundome::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player2_Sundome"))
	{
		ImGui::DragFloat2("SpritePos", &spritePos.x, 0.1f);
		ImGui::DragFloat2("SpriteScale", &spriteScale.x);

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