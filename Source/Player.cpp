#include <imgui.h>
#include <cmath>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectilePlayer.h"
#include "EnemySphere.h"

static Player* instance = nullptr;

// インスタンス取得
Player& Player::Instance()
{
	return *instance;
}

Player::Player()
{
	// インスタンスポインタ設定
	instance = this;

	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	//model = new Model("Data/Model/Jammo/Jammo.mdl");
	model[0] = new Model("Data/Model/Character/charactor01.mdl");
	model[1] = new Model("Data/Model/Character/charactor02.mdl");

	Audio& audioManager = Audio::Instance();
	sound[0] = audioManager.LoadAudioSource("Data/Audio/剣の素振り1.wav");
	sound[1] = audioManager.LoadAudioSource("Data/Audio/ぐお.wav");;
	sound[2] = audioManager.LoadAudioSource("Data/Audio/死亡.wav");;

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	attackEffect = std::make_unique<Effect>("Data/Effect/PlayerAttack.efk");

	// 待機ステートへ遷移
	TransitionIdleState();

	health = damageHealth = maxHealth = 200;

	maxWaitCount = 5;

	radius = 0.35f;
	height = 1.5;
}

// デストラクタ
Player::~Player()
{
	delete hitEffect;

	for (int i = 0; i < 2; i++)
	{
		delete model[i];
	}
}

// 更新処理
void Player::Update(float elapsedTime)
{
	// ステート毎の処理
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
	break;
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	//case State::Jump:
	//	UpdateJumpState(elapsedTime);
	//	break;
	//case State::Land:
	//	UpdateLandState(elapsedTime);
	//	break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	//case State::Revive:
	//	UpdateReviveState(elapsedTime);
	//	break;
	}

	// 速力更新処理
	UpdateVelocity(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// 弾丸と敵に衝突処理
	CollisionProjectilesVsEnemies();

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// モデルアニメーション更新処理
	model[0]->UpdateAnimation(elapsedTime);
	model[1]->UpdateAnimation(elapsedTime);

	// モデル更新処理
	model[0]->UpdateTransform(transform);
	model[1]->UpdateTransform(transform);

	// 位置制御
	PositionControll();

	// HP制御
	HpControll();
}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model[0]);
	shader->Draw(dc, model[1]);

	// 弾丸描画処理
	projectileManager.Render(dc, shader);
}

// 位置調整
void Player::PositionControll()
{
	position.z = 0;
	if(position.x < -6.66) position.x = -6.66;
	if (position.x > 6.6) position.x = 6.6;
}

// 移動入力処理
bool Player::InputMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	// 旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return !(moveVec.x == 0.0f && moveVec.z == 0.0f);
}

 // TODO:弾丸入力処理
//void Player::InputProjectile()
//{
//	GamePad& gamePad = Input::Instance().GetGamePad();
//
//	 直進弾丸発射
//	if (gamePad.GetButtonDown() & GamePad::BTN_X) //Cキー
//	{
//		 前方向
//		DirectX::XMFLOAT3 dir;
//
//		dir.x = transform._31;
//		dir.y = transform._32;
//		dir.z = transform._33;
//		
//		DirectX::XMVECTOR DIR;
//		DIR = DirectX::XMLoadFloat3(&dir);
//		DIR = DirectX::XMVector3Normalize(DIR);
//		DirectX::XMStoreFloat3(&dir, DIR);
//
//		 発射位置(プレイヤーの腰あたり)
//		DirectX::XMFLOAT3 pos;
//		pos.x = position.x;
//		pos.y = position.y + (height / 2);
//		pos.z = position.z;
//
//		 ターゲット(デフォルトではプレイヤーの前方)
//		DirectX::XMFLOAT3 target;
//		target.x = pos.x + dir.x * 10.0f; // 10.0f は任意の距離
//		target.y = pos.y + dir.y * 10.0f;
//		target.z = pos.z + dir.z * 10.0f;
//
//		 一番近くの敵をターゲット
//		float dist = FLT_MAX;
//		EnemyManager& enemyManager = EnemyManager::Instance();
//		int enemyCount = enemyManager.GetEnemyCount();
//		for (int i = 0; i < enemyCount; ++i)
//		{
//			 敵との距離判定
//			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
//			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
//			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
//			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
//			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
//			float d;
//			DirectX::XMStoreFloat(&d, D);
//			if (d < dist)
//			{
//				dist = d;
//				target = enemy->GetPosition();
//				target.y += enemy->GetHeight() * 0.5f;
//			}
//		}
//
//		発射
//		ProjectileStraight * projectile = new ProjectileStraight(&projectileManager);
//		projectile->Launch(dir, pos);
//		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
//		projectile->Launch(dir, pos);
//		
//		projectileManager.Register(projectile);
//	}
//}

// 攻撃入力処理
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}

	return false;
}

// 待機ステートへ遷移
void Player::TransitionIdleState()
{
	state = State::Idle;

	// 待機アニメーション再生
	model[0]->PlayAnimation(2, true);
	model[1]->PlayAnimation(2, true);
}

// 待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime)
{
	// 移動入力処理
	// 移動入力されたら移動ステートへ遷移
	
	const GamePadButton ArrowButton =
		GamePad::BTN_LEFT |
		GamePad::BTN_RIGHT;

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonHeld() & ArrowButton)
	{
		// 移動ステートへ遷移
		TransitionMoveState();
	}

	InputMove(elapsedTime);

	// ジャンプ入力処理
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// ジャンプステートへ遷移
		playerAnimeCount = 0;
		//TransitionJumpState();
	}

	//InputJump();

	// 弾丸入力処理
	//InputProjectile();

	playerMove = 0;

	// 攻撃入力処理
	if (gamePad.GetButtonDown() & GamePad::BTN_START || gamePad.GetButtonDown() & GamePad::BTN_SPACE)
	{
		// 攻撃ステートへ遷移
		playerAnimeCount = 0;
		TransitionAttackState();
	}
}

// 移動ステートへ遷移
void Player::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	model[0]->PlayAnimation(0, true);
	model[1]->PlayAnimation(0, true);
}

// 移動ステート更新処理
void Player::UpdateMoveState(float elapsedTime)
{
	// 移動入力処理
	const GamePadButton ArrowButton =
		GamePad::BTN_UP |
		GamePad::BTN_LEFT |
		GamePad::BTN_RIGHT |
		GamePad::BTN_DOWN;

	GamePad& gamePad = Input::Instance().GetGamePad();

	if(!(gamePad.GetButtonHeld() & ArrowButton) && playerMove == 8)
	{
		// 待機ステートへ遷移
		TransitionIdleState();
	}

	InputMove(elapsedTime);

	//// ジャンプ入力処理
	//if (gamePad.GetButtonDown() & GamePad::BTN_A)
	//{
	//	// ジャンプステートへ遷移
	//	playerAnimeCount = 0;
	//	TransitionJumpState();
	//}

	//InputJump();

	// 弾丸入力処理
	/*InputProjectile();*/

	// プレイヤー移動カウント
	playerMove++;

	if (playerMove > 8) playerMove = 8;

	// 攻撃入力処理
	if (gamePad.GetButtonDown() & GamePad::BTN_START || gamePad.GetButtonDown() & GamePad::BTN_SPACE)
	{
		// 攻撃ステートへ遷移
		playerAnimeCount = 0;
		TransitionAttackState();
	}
}

//// ジャンプステートへ遷移
//void Player::TransitionJumpState()
//{
//	state = State::Jump;
//
//	// ジャンプアニメーション再生
//	model->PlayAnimation(Anim_Jump, false);
//}

//// ジャンプステート更新処理
//void Player::UpdateJumpState(float elapsedTime)
//{
//	// ジャンプ入力処理
//	InputMove(elapsedTime);
//
//	// ジャンプ入力処理
//	//InputJump();
//
//	// 弾丸入力処理
//	InputProjectile();
//}

//// 着地ステートへ遷移
//void Player::TransitionLandState()
//{
//	state = State::Land;
//
//	// 着地アニメーション再生
//	model->PlayAnimation(Anim_Landing, false);
//}
//
//// 着地ステート更新処理
//void Player::UpdateLandState(float elapsedTime)
//{
//	if (velocity.y == 0 && playerAnimeCount < 40)
//	{
//		playerAnimeCount++;
//	}
//
//	if (playerAnimeCount >= 40)
//	{
//		// 待機ステートへ遷移
//		TransitionIdleState();
//	}
//}

// 攻撃ステートへ遷移
void Player::TransitionAttackState()
{
	state = State::Attack;

	sound[0]->Play(false, 0.65f);
	// 着地アニメーション再生
	model[0]->PlayAnimation(1, false);
	model[1]->PlayAnimation(1, false);

	DirectX::XMFLOAT3 e = position;
	e.y += height * 0.5f;

	attackEffect->Play(e, 0.2f);
}

// 攻撃ステート更新処理
void Player::UpdateAttackState(float elapsedTime)
{
	if (playerAnimeCount < 45)
	{
		playerAnimeCount++;
	}

	if (playerAnimeCount >= 45)
	{
		// 待機ステートへ遷移
		TransitionIdleState();
	}

	// 任意のアニメーション再生区間でのみ衝突処理判定をする
	float animationTime = static_cast<float>(playerAnimeCount) / 45.0f;
	if (animationTime >= 0.3f && animationTime <= 0.7f)
		attackCollisionFlag = true;
	else 
		attackCollisionFlag = false;
}

// TODO:ノードと敵の衝突処理
//void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
//{
//	// ノード取得
//	Model::Node* node = model[]->FindNode(nodeName);
//
//	// ノードが存在しない場合は処理を終了
//	if (!node) return;
//
//	// ノード位置取得
//	DirectX::XMMATRIX nodeTransform = DirectX::XMLoadFloat4x4(&node->worldTransform);
//	DirectX::XMVECTOR nodePosVector = nodeTransform.r[3];
//	DirectX::XMFLOAT3 nodePosition;
//	DirectX::XMStoreFloat3(&nodePosition, nodePosVector);
//
//	// 敵マネージャーのインスタンスを取得
//	EnemyManager& enemyManager = EnemyManager::Instance();
//
//	int pm = projectileManager.GetProjectileCount();
//
//	// 指定のノードと全ての敵を総当たりで衝突処理
//	//int enemyCount = enemyManager.GetEnemyCount();
//	for (int i = 0; i < pm; ++i)
//	{
//		Projectile* projectile = projectileManager.GetProjectile(i);
//
//		// 衝突処理
//		DirectX::XMFLOAT3 outPosition;
//		if (Collision::IntersectSphereVsSphere(
//			nodePosition,
//			nodeRadius,
//			projectile->GetPosition(),
//			projectile->GetRadius(),
//			outPosition
//		))
//		{
//			EnemyManager& enemyManager = EnemyManager::Instance();
//			int enemyCount = enemyManager.GetEnemyCount();
//			for (int i = 0; i < enemyCount; ++i)
//			{
//				Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
//				const DirectX::XMFLOAT3& enemyPosition = enemy->GetPosition();
//
//				// 前方向
//				DirectX::XMFLOAT3 dir = { 0,0,0 };
//
//				dir.x = enemyPosition.x - position.x;
//				dir.y = enemyPosition.y - position.y;
//				dir.z = enemyPosition.z - position.z;
//
//				DirectX::XMVECTOR DIR;
//				DIR = DirectX::XMLoadFloat3(&dir);
//				DIR = DirectX::XMVector3Normalize(DIR);
//				DirectX::XMStoreFloat3(&dir, DIR);
//
//				// 発射位置(プレイヤーの腰あたり)
//				DirectX::XMFLOAT3 pos = {0,0,0};
//				pos.x = position.x;
//				pos.y = position.y + 1;
//				pos.z = position.z;
//
//				DirectX::XMFLOAT3 target = { 0,0,0 };
//
//				ProjectilePlayer* projectile = new ProjectilePlayer(&projectileManager);
//				projectile->Launch(dir, pos);
//			}
//		
//		}
//	}
//}

// ダメージステートへ遷移
void Player::TransitionDamageState()
{
	state = State::Damage;
	waitCount = 0;
	sound[1]->Play(false, 1.0f);
	// ダメージアニメーション再生
	model[0]->PlayAnimation(3, false);
	model[1]->PlayAnimation(3, false);
}

// ダメージステート更新処理
void Player::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (waitCount > 20)
	{
		TransitionIdleState();
	}
	else
	{
		waitCount++;
	}
}

// 死亡ステートへ遷移
void Player::TransitionDeathState()
{
	state = State::Death;

	sound[2]->Play(false, 1.0f);

	// 死亡アニメーション再生
	model[0]->PlayAnimation(4, false);
	model[1]->PlayAnimation(4, false);
}

// 死亡ステート更新処理
void Player::UpdateDeathState(float elapsedTimae)
{
	if (!model[0]->IsPlayAnimation())
	{
		//// ボタンを押したら復活ステートへ遷移
		//GamePad& gamePad = Input::Instance().GetGamePad();
		//if (gamePad.GetButtonDown() & GamePad::BTN_A)
		//{
		//	TransitionReviveState();
		//}
	}
}

//// 復活ステートへ遷移
//void Player::TransitionReviveState()
//{
//	state = State::Revive;
//
//	// 体力回復
//	health = maxHealth;
//
//	// 復活アニメーション再生
//	model->PlayAnimation(Anim_Reving, false);
//}

//// 復活ステート更新処理
//void Player::UpdateReviveState(float elapsedTime)
//{
//	// 復活アニメーション終了後に待機ステートへ遷移
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleState();
//	}
//}

// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		//if (Collision::IntersectSphereVsSphere(
		//	Player::GetPosition(),
		//	Player::GetRadius(),
		//	enemy->GetPosition(),
		//	enemy->GetRadius(),
		//	outPosition
		//))
		//{
		//	// 押し出しの後の位置設定
		//	enemy->SetPosition(outPosition);
		//}

		
		if (Collision::IntersectCylinderVsCylinder(
			Player::GetPosition(),
			Player::GetRadius(),
			Player::GetHeight(),
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
			//// プレイヤーが敵の上にいるかを判定する
			//float diff = Player::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player::Jump(10);
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
			//// 上から踏んづけた場合は小ジャンプする
			//if (normal.y > 0.8f)
			//{
			//	// 小ジャンプする
			//	Jump(jumpSpeed * 0.5f);
			//}
			//else
			//{
			//	// 押し出し後の位置設定
			//	enemy->SetPosition(outPosition);
			//}
			
		}

	}
}

//デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// 衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

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
	//if (attackCollisionFlag)
	//{
	//	Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
	//	debugRenderer->DrawSphere(DirectX::XMFLOAT3(
	//	leftHandBone->worldTransform._41,
	//	leftHandBone->worldTransform._42,
	//	leftHandBone->worldTransform._43),
	//	leftHandRadius,
	//	DirectX::XMFLOAT4(1, 0, 0, 1)
	//	);
	//}
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
	// 下方向の速力が一定以上なら着地ステートへ
	//if (velocity.y <= -5)
	//{
	//	TransitionLandState();
	//}

	// ダメージ、死亡ステート時は着地した時にステート遷移しないようにする
	if (velocity.y <= -5 && state != State::Damage && state != State::Death)
	{
		//TransitionLandState();
	}

	jumpCount = 0;
}

// ダメージを受けた時に呼ばれる
void Player::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// ジャンプ入力処理
//bool Player::InputJump()
//{
//	GamePad& gamePad = Input::Instance().GetGamePad();
//	{
//		if (gamePad.GetButtonDown() & GamePad::BTN_A) //Zキー
//		{
//			// ジャンプ回数制限
//			if (jumpCount < jumpLimit)
//			{
//				// ジャンプ
//				Jump(jumpSpeed);
//				jumpCount++;
//
//				// ジャンプ入力した
//				return true;
//			}
//		}
//		return false;
//	}
//}

// 弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
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
			Enemy* enemy = enemyManager.GetEnemy(j);

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
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// 吹き飛ばす
					{
						// 衝突点から敵の中心点へのベクトルを計算
						DirectX::XMVECTOR enemyPos = DirectX::XMLoadFloat3(&enemy->GetPosition());
						DirectX::XMVECTOR impactPos = DirectX::XMLoadFloat3(&position);
						DirectX::XMVECTOR impulseDir = DirectX::XMVectorSubtract(enemyPos, impactPos);

						// 正規化して方向ベクトルを得る
						impulseDir = DirectX::XMVector3Normalize(impulseDir);

						// 望むインパルスの強さを設定
						float impulseStrength = 10.0f; // この値は調整が必要です
						DirectX::XMVECTOR impulse = DirectX::XMVectorScale(impulseDir, impulseStrength);

						// インパルスをXMFLOAT3に変換
						DirectX::XMFLOAT3 impulseFloat3;
						DirectX::XMStoreFloat3(&impulseFloat3, impulse);

						enemy->AddImpulse(impulseFloat3);
					}

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
void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::TreeNode("Player"))
	{
		ImGui::InputFloat3("Velocity", &velocity.x);

		ImGui::DragInt("HP", &health);

		// トランスフォーム
		if(ImGui::CollapsingHeader("Transform"))
			// 位置
			ImGui::InputFloat3("Position", &position.x);
			// 回転
			DirectX::XMFLOAT3 a ={ 0,0,0 };
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::InputFloat3("Scale", &scale.x);
			ImGui::TreePop();
	}
}