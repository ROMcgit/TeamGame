#include <imgui.h>
#include "Player3_SoratobuHusenWari.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Camera/CameraController.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Audio/SoundManager.h"

static Player3_SoratobuHusenWari* instance = nullptr;

// インスタンス取得
Player3_SoratobuHusenWari& Player3_SoratobuHusenWari::Instance()
{
	return *instance;
}

Player3_SoratobuHusenWari::Player3_SoratobuHusenWari()
{
	// インスタンスポインタ設定
	instance = this;

	// モデル読み込み
	model = std::make_unique <Model>("Data/Model/3.SoratobuHusenWari/Player/Player.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.022f;

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 1.1f;
	height = 3.1f;

	collisionOffset.z = -0.17f;

	opacity = 0.8f;

	// 重力
	gravity = gravityReset = 0.25f;

	//! プレイヤーUI
	//uiSprite[0] = std::make_unique <Sprite>("Data/Sprite/5.UI/CuppyUI.png");
	for (int i = 0; i < 3; i++)
	{
		uiSprite[i + 1] = std::make_unique <Sprite>(); // HPゲージ
		//[1] 灰色
		//[2] 赤色
		//[3] 緑色
	}

	hp = maxHp = hpDamage = 200;

	hpImagePos = { 8.0f, 15 };
	hpImageShakePosY = hpImagePos.y;

	hpSpritePos = { 15, 15 };
	hpSpriteShakePosY = hpSpritePos.y;

	playerHpSpriteWidth = 200;

	SoundManager& sound = SoundManager::Instance();
	sound.LoadSound("着地", "Data/Audio/Sound/Attack.wav");
	sound.LoadSound("雷", "Data/Audio/Sound/Thunder.wav");

	// 移動ステートへ遷移
	TransitionMoveState();
}

// デストラクタ
Player3_SoratobuHusenWari::~Player3_SoratobuHusenWari()
{
}

// 更新処理
void Player3_SoratobuHusenWari::Update(float elapsedTime)
{
	position.z = 0.0f;

	if (CameraController::debugCamera) return;

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
			state = State::Move; // ステートを待機に変更
			model->PlayAnimation(movieAnimNum, movieAnimLoop);
			movieAnimation = true;
		}
	}
	// ムービー中では無い時
	else
		movieAnimation = false; // ムービー中に待機ステートかどうか

	// キャラクター状態更新処理
	UpdateGameObjectBaseState(elapsedTime);

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// モデル更新処理
	model->UpdateTransform(transform);

	if (G3_SoratobuHusenWari::gameTimer < 2.0f)
	{
		gravity = 0.0f;

		return;
	}
	else if(!gameStart)
	{
		gravity = gravityReset;

		gameStart = true;
	}

	if (!movieScene)
	{
		// ステート毎の処理
		switch (state)
		{
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

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// 衝突攻撃更新処理
	collisionAttackManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayer3_SoratobuHusenWariVsEnemies();

	// 位置制限
	PositionControll(elapsedTime);

	UpdateHpPinch(elapsedTime);
}

// 描画処理
void Player3_SoratobuHusenWari::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);

	// 弾丸描画処理
	projectileManager.Render(dc, shader);

	// 衝突攻撃描画処理
	collisionAttackManager.Render(dc, shader);
}

// HPなどのUI描画
void Player3_SoratobuHusenWari::SpriteRender(ID3D11DeviceContext* dc)
{
	float textureWidth = static_cast<float>(uiSprite[1]->GetTextureWidth());
	float textureHeight = static_cast<float>(uiSprite[1]->GetTextureHeight());

	if (!hideSprites)
	{
		// ゲージの裏(灰色)
		uiSprite[1]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			playerHpSpriteWidth, 40,
			0, 0, textureWidth, textureHeight,
			0,
			0.3f, 0.3f, 0.3f, 1);

		float hpWidth = (static_cast<float>(hp) / maxHp) * playerHpSpriteWidth; // HPの横の長さ
		float hpDamageWidth = (static_cast<float>(hpDamage) / maxHp) * playerHpSpriteWidth; // HPダメージの横の長さ

		// ダメージ(赤色)
		uiSprite[2]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			hpDamageWidth, 40,
			0, 0, textureWidth, textureHeight,
			0,
			1, 0, 0, 1);

		// HPゲージ
		uiSprite[3]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			hpWidth, 40,
			0, 0, textureWidth, textureHeight,
			0,
			hpSpriteColorP.x,
			hpSpriteColorP.y,
			hpSpriteColorP.z,
			1);

		//textureWidth = static_cast<float>(uiSprite[0]->GetTextureWidth());
		//textureHeight = static_cast<float>(uiSprite[0]->GetTextureHeight());

		// プレイヤーUI画像
		/*uiSprite[0]->Render(dc,
			hpImagePos.x, hpImagePos.y,
			241, 84,
			0, 0, textureWidth, textureHeight,
			0,
			hpImageColor.x,
			hpImageColor.y,
			hpImageColor.z,
			1);*/
	}
}

// 位置制限
void Player3_SoratobuHusenWari::PositionControll(float elapsedTime)
{
	//! 位置Xを制限
	if (position.x < -10.2f || position.x > 10.2f)
	{
		velocity.x = 0;
		position.x = std::clamp(position.x, -10.2f, 10.2f);
	}

	if (position.y > 25.0f)
	{
		position.y = 25.0f;

		velocity.y -= 5 * elapsedTime;
	}
}

// 移動入力処理
bool Player3_SoratobuHusenWari::InputMove(float elapsedTime)
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

// HPピンチ更新処理
void Player3_SoratobuHusenWari::UpdateHpPinch(float elapsedTime)
{
	//! HPがピンチじゃないなら、処理を止める
	if (hp > maxHp * 0.28f || hpShake)
	{
		hpPinchTimer = 0.0f; // タイマーを0にする

		//! マテリアルの色を1にする
		materialColor.x = materialColor.y = materialColor.z = 1.0f;

		//! エミッシブの強さを変更
		if (emissiveStrength > 0.0f && !emissivePhaseChange)
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(0, 0, 0), 0.0f, 0.1f);

		return;
	}

	//! タイマーを増やす
	hpPinchTimer += elapsedTime;

	//! タイマーに基づいて色を切り替え
	const float blinkInterval = 0.15f; //! 点滅間隔
	if (fmod(hpPinchTimer, blinkInterval * 2) < blinkInterval) {
		hpSpriteColorP = { 1.0f, 0.412f, 0.706f }; // ピンク色
	}
	else
	{
		hpSpriteColorP = { 1.0f, 1.0f, 1.0f }; // 白色
	}

	//! エミッシブの強さを変更する
	if (emissiveStrength == 0.0f)
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 1.0f, 0.2f);
	//! 0じゃないなら
	else
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.2f);
}

// 移動ステートへ遷移
void Player3_SoratobuHusenWari::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	model->PlayAnimation(Anim_Move, true);
}

// 移動ステート更新処理
void Player3_SoratobuHusenWari::UpdateMoveState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	float move = 0;

	if (G3_SoratobuHusenWari::gameTimer < 30.0f)
		move = 5.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
		move = 8.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
		move = 12.0f;
	else
		move = 17.0f;


	float angleZ           = 10;
	float angleZChangeTime = 0.2f;
	if ((gamePad.GetButtonHeld() & GamePad::BTN_LEFT) && !(gamePad.GetButtonHeld() & GamePad::BTN_RIGHT))
	{
#if 1
		velocity.x = -move;

		//! 角度の変更を解除
		SetAngleChangeUnlock();

		//! 角度Zを変更する
		SetAngleZChange(DirectX::XMConvertToRadians(angleZ), angleZChangeTime);
#endif
	}
	else if ((gamePad.GetButtonHeld() & GamePad::BTN_RIGHT) && !(gamePad.GetButtonHeld() & GamePad::BTN_LEFT))
	{
#if 1
		velocity.x = move;

		//! 角度の変更を解除
		SetAngleChangeUnlock();

		//! 角度Zを変更する
		SetAngleZChange(DirectX::XMConvertToRadians(-angleZ), angleZChangeTime);
#endif
	}
	else
	{
#if 1
		velocity.x = 0;

		//! 角度変更を解除
		SetAngleChangeUnlock();

		//! 角度Zを変更する
		SetAngleZChange(DirectX::XMConvertToRadians(0), angleZChangeTime);
#endif
	}

	GamePadButton button =
	GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y | GamePad::BTN_SPACE;
	if (gamePad.GetButtonHeld() & button)
	{
		velocity.y += 100 * elapsedTime;
	}

	velocity.y = std::clamp(velocity.y, -5.0f, 5.0f);


	if (isDamage || isGround)
	{
		int damage = isDamage ? 40 : 20;
		//! ダメージを与える
		ApplyDamage(damage, 0.1f);

		std::string soundPath = isDamage ? "雷" : "着地";
		float volume = isDamage ? 1.5f : 1.0f;
		SoundManager::Instance().PlaySound(soundPath.c_str(), volume);

		TransitionDamageState();
	}
}

// ダメージステートへ遷移
void Player3_SoratobuHusenWari::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_Damage, false);

	hpShake = true;

	if(!isGround)
		velocity.y = 0.0f;
	else
		velocity.y = 10.0f;

	gravity = 0.13f;

	//! カラーフィルターを変更する
	SetColorFilterChange(DirectX::XMFLOAT3(Camera::colorFilterReset.x + 1.7f, Camera::colorFilterReset.y, Camera::colorFilterReset.z), 0.5f);

	//! カメラシェイク
	SetCameraShake(0.2f, DirectX::XMINT3(10, 60, 0));

	stateChangeWaitTimer = 1.2f;
}

// ダメージステート更新処理
void Player3_SoratobuHusenWari::UpdateDamageState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (stateChangeWaitTimer <= 0.0f)
	{
		gravity = gravityReset;

		//! カラーフィルターを戻す
		SetColorFilterResetChange(0.3f);

		//! 移動ステートに遷移
		TransitionMoveState();

		isDamage = false;
	}
}

// 死亡ステートへ遷移
void Player3_SoratobuHusenWari::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player3_SoratobuHusenWari::UpdateDeathState(float elapsedTimae)
{
}

// プレイヤーとエネミーとの衝突処理
void Player3_SoratobuHusenWari::CollisionPlayer3_SoratobuHusenWariVsEnemies()
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
		//	Player3_SoratobuHusenWari::GetPosition(),
		//	Player3_SoratobuHusenWari::GetRadius(),
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
			//float diff = Player3_SoratobuHusenWari::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player3_SoratobuHusenWari::Jump(10);
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
void Player3_SoratobuHusenWari::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(collisionPos, radius, height,{ debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// 着地した時に呼ばれる
void Player3_SoratobuHusenWari::OnLanding()
{
}

// ダメージを受けた時に呼ばれる
void Player3_SoratobuHusenWari::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player3_SoratobuHusenWari::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}

// 当たり判定描画
void Player3_SoratobuHusenWari::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player3_SoratobuHusenWari"))
	{
		ImGui::InputFloat3("Velocity", &velocity.x);

		ImGui::DragInt("HP", &hp);
		ImGui::DragFloat2("HPPos", &hpSpritePos.x, 0.2f);
		ImGui::DragFloat("HPWidth", &playerHpSpriteWidth, 0.2f);

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