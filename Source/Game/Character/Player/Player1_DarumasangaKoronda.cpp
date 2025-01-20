#include <imgui.h>
#include "Player1_DarumasangaKoronda.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"

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
	model = std::make_unique <Model>("Data/Model/Jammo/Jammo.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// ヒットエフェクト読み込み
	hitEffect = std::make_unique <Effect>("Data/Effect/Hit.efk");

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

	// 速力更新処理
	UpdateVelocity(elapsedTime);

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayer1_DarumasangaKorondaVsEnemies();

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// 弾丸と敵に衝突処理
	CollisionProjectilesVsEnemies();

	// 当たり判定の位置を設定
	CollisionPosSettings();

	// オブジェクト行列を更新
	UpdateTransform();

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
	Turn3D(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return !(moveVec.x == 0.0f && moveVec.z == 0.0f);
}

// 攻撃入力処理
bool Player1_DarumasangaKoronda::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}

	return false;
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

	InputMove(elapsedTime);

	// ジャンプ入力処理
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// ジャンプステートへ遷移
		TransitionJumpState();
	}

	InputJump();
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

	if(gamePad.GetAxisLX() == 0 && gamePad.GetAxisLY() == 0)
	{
		// 待機ステートへ遷移
		TransitionWaitState();
	}

	InputMove(elapsedTime);

	// ジャンプ入力処理
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// ジャンプステートへ遷移
		TransitionJumpState();
	}

	InputJump();
}

// ジャンプステートへ遷移
void Player1_DarumasangaKoronda::TransitionJumpState()
{
	state = State::Jump;

	// ジャンプアニメーション再生
	model->PlayAnimation(Anim_Jump, false);
}

// ジャンプステート更新処理
void Player1_DarumasangaKoronda::UpdateJumpState(float elapsedTime)
{
	// ジャンプ入力処理
	InputMove(elapsedTime);

	// ジャンプ入力処理
	InputJump();
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

// プレイヤーとエネミーとの衝突処理
void Player1_DarumasangaKoronda::CollisionPlayer1_DarumasangaKorondaVsEnemies()
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
		//	Player1_DarumasangaKoronda::GetPosition(),
		//	Player1_DarumasangaKoronda::GetRadius(),
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
			//float diff = Player1_DarumasangaKoronda::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player1_DarumasangaKoronda::Jump(10);
			//	// 小ジャンプさせるためにY方向の速度を設定する
			//}

			//// 押し出しの後の位置設定
			//enemy->SetPosition(outPosition);

			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P,E);
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
void Player1_DarumasangaKoronda::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// 衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

	//Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
	//debugRenderer->DrawSphere(DirectX::XMFLOAT3(
	//	leftHandBone->worldTransform._41,
	//	leftHandBone->worldTransform._42,
	//	leftHandBone->worldTransform._43),
	//	leftHandRadius,
	//	DirectX::XMFLOAT4(1, 0, 0, 1)
	//);

	// 攻撃衝突用の左手ノードのデバッグ球を描画
	if (attackCollisionFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(
		leftHandBone->worldTransform._41,
		leftHandBone->worldTransform._42,
		leftHandBone->worldTransform._43),
		leftHandRadius,
		DirectX::XMFLOAT4(1, 0, 0, 1)
		);
	}
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

// ジャンプ入力処理
bool Player1_DarumasangaKoronda::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_A) //Zキー
		{
			// ジャンプ回数制限
			if (jumpCount < jumpLimit)
			{
				// ジャンプ
				Jump(jumpSpeed);
				jumpCount++;

				// ジャンプ入力した
				return true;
			}
		}
		return false;
	}
}

// 弾丸と敵の衝突処理
void Player1_DarumasangaKoronda::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f, 0))
				{
					// ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					// 弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
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