#include "SikaTentyo.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player.h"
#include "Other/Collision.h"
#include "Game/Scene/SceneTitle.h"

// コンストラクタ
SikaTentyo::SikaTentyo()
{
	model = std::make_unique<Model>("Data/Model/SikaTentyo/SikaTentyo.mdl");

	roarEffect = std::make_unique<Effect>("Data/Effect/Effect/Roar.efk");

	scale.x = scale.y = scale.z = 0.04f;

	gravity = 0.0f;

	angle.y = DirectX::XMConvertToRadians(180);

	hp = hpDamage = 0;
	maxHp = 500;

	collisionOffset = { -0.1f, 0.5f, 0 };

	radius = 2;
	height = 6;

	//! HPゲージの位置
	hpSpritePos = { 558.5f, 663.1f };
	hpSpriteShakePosY = hpSpritePos.y;

	//! HP画像の位置
	hpImagePos = { 550.0f, 661.0f };
	hpImageShakePosY = hpImagePos.y;

	hpSpriteAdjust = 1.38f;

	//! UIを表示しない
	hideSprites = true;

	//! HPを減らす演出までの時間
	hpDamageDirectorWaitCount = maxHpDamageDirectorWaitCount = 0.005f;

	name = std::make_unique<Sprite>("Data/Sprite/シカテンチョウ.png");

	//! HP描画
	hpSprite[0] = std::make_unique<Sprite>("Data/Sprite/ボスHP.png");
	hpSprite[1] = std::make_unique<Sprite>(); //黒色
	hpSprite[2] = std::make_unique<Sprite>(); //黄色
	hpSprite[3] = std::make_unique<Sprite>(); //紫色

	TransitionEntryState();
}

// デストラクタ
SikaTentyo::~SikaTentyo()
{
}

// 更新処理
void SikaTentyo::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Entry:
		UpdateEntryState(elapsedTime);
		break;
	case State::Start:
		UpdateStartState(elapsedTime);
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

	// HP管理
	HpControll(elapsedTime);

	// 当たり判定の位置設定
	CollisionPosSettings();

	// HPシェイク
	UpdateHpShake(elapsedTime);

	// スケール変更更新処理
	UpdateScaleChange(elapsedTime);

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// プレイヤーとの当たり判定
	CollisionEnemyVsPlayer();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void SikaTentyo::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HPなどの描画
void SikaTentyo::SpriteRender(ID3D11DeviceContext* dc)
{
	float textureWidth = static_cast<float>(hpSprite[1]->GetTextureWidth());
	float textureHeight = static_cast<float>(hpSprite[1]->GetTextureHeight());

	// バトル１
	if (!movieScene && !hideSprites)
	{
		// ゲージの裏(濃い灰色)
		hpSprite[1]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			690, hpSpriteHeight * 0.9f,
			0, 0,
			textureWidth, textureHeight,
			0,
			0.2f, 0.2f, 0.2f, 1);

		// ゲージ(黄色)
		hpSprite[2]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			hpDamage * hpSpriteAdjust, 36.0f,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 0, 1);

		// ゲージ(紫色)
		hpSprite[2]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			hp * hpSpriteAdjust, 36.0f,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 0, 1, 1);

		//! HP画像
		textureWidth = static_cast<float>(hpSprite[0]->GetTextureWidth());
		textureHeight = static_cast<float>(hpSprite[0]->GetTextureHeight());

		// HPゲージ
		hpSprite[0]->Render(dc,
			hpImagePos.x, hpImagePos.y,
			700, hpSpriteHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			hpSpriteColor.x,
			hpSpriteColor.y,
			hpSpriteColor.z,
			1);

		textureWidth = static_cast<float>(name->GetTextureWidth());
		textureHeight = static_cast<float>(name->GetTextureHeight());

		//! 名前
		name->Render(dc,
			hpImagePos.x + 410, hpImagePos.y - 45,
			232, 46,
			0, 0,
			textureWidth, textureHeight,
			0,
			hpSpriteColor.x,
			hpSpriteColor.y,
			hpSpriteColor.z,
			nameSpriteOpacity);
	}
}

// HP表示
void SikaTentyo::RenderEnemyGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// 敵の位置を取得
	DirectX::XMFLOAT3 enemyPos = position;

	// 位置を変換
	DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&enemyPos);
	DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project(
		Pos,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World);

	// スクリーン座標を取得
	DirectX::XMFLOAT3 screenPos;
	DirectX::XMStoreFloat3(&screenPos, ScreenPos);

	// HPゲージの描画位置
	float gaugeWidth = hp * 8; // HPゲージの幅
	float gaugeHeight = 5.0f;      // HPゲージの高さ
	float gaugeX = screenPos.x - gaugeWidth / 2;
	float gaugeY = screenPos.y - gaugeHeight - 55.0f; // 少し上にオフセット

	DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Unproject(
		ScreenPos,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	DirectX::XMFLOAT3 worldPos;
	DirectX::XMStoreFloat3(&worldPos, WorldPosition);

	//enemyHp->Render(dc,
	//	gaugeX, //dx
	//	gaugeY, //dy
	//	gaugeWidth, //dw
	//	gaugeHeight, //dh
	//	0,           //sx
	//	0,           //sy
	//	gaugeWidth,  //sw
	//	gaugeHeight, //sh
	//	0,			 //angle
	//	1, 0, 0, 1   //color
	//);
}

// デバッグプリミティブ描画
void SikaTentyo::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	// ターゲット位置をデバッグ球描画
	debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));
}

// デバッグ用GUI描画
void SikaTentyo::DrawDebugGUI()
{
	if (ImGui::TreeNode("SikaTentyo"))
	{
		ImGui::InputFloat("Dist", &dist);

		if (ImGui::CollapsingHeader("Hp", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputInt("HP", &hp);
			ImGui::InputInt("HPDamage", &hpDamage);
			ImGui::DragFloat2("HpImagePos", &hpImagePos.x);
			ImGui::DragFloat2("HpSpritePos", &hpSpritePos.x, 0.1f);
			ImGui::DragFloat("HpSpriteAdjust", &hpSpriteAdjust, 0.01f);
			ImGui::Checkbox("HpDirectorFinished", &hpDirectorFinished);
		}

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

// プレイヤーとの接触処理
void SikaTentyo::CollisionEnemyVsPlayer()
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
		if (player.ApplyDamage(player.GetViewMoveSpeed() + 25, 1.0f))
		{
			// 前方向
			DirectX::XMFLOAT3 velocity;

			if (position.x > player.GetPosition().x)
				velocity.x = -80;
			else
				velocity.x = 80;

			if (position.z > player.GetPosition().z)
				velocity.z = -80;
			else
				velocity.z = 80;

			player.SetVelocity(DirectX::XMFLOAT3(velocity.x, 100.0f, velocity.z));
		}
		else if (player.GetLunges() && invincibleTimer <= 0.0f)
		{
			this->ApplyDamage(1, 1.0f);
		}
	}
}

// ノードとプレイヤーの衝突処理
void SikaTentyo::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	// ノードの位置と当たり判定を行う
	Model::Node* node = model->FindNode(nodeName);
	if (node != nullptr)
	{
		// ノードのワールド座標
		DirectX::XMFLOAT3 nodePosition(
			node->worldTransform._41,
			node->worldTransform._42,
			node->worldTransform._43
		);

		// 当たり判定表示
		Graphics::Instance().GetDebugRenderer()->DrawSphere(
			nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
		);

		// プレイヤーと当たり判定
		Player& player = Player::Instance();
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition))
		{
			// ダメージを与える
			if (player.ApplyDamage(1, 0.5f))
			{
				// 敵を吹っ飛ばすベクトルを算出
				DirectX::XMFLOAT3 vec;
				vec.x = outPosition.x - nodePosition.x;
				vec.z = outPosition.z - nodePosition.z;
				float length = sqrtf(vec.x * vec.x + vec.z * vec.z);
				vec.x /= length;
				vec.z /= length;

				// XZ平面に吹っ飛ばす力をかける
				float power = 10.0f;
				vec.x *= power;
				vec.z *= power;
				// Y方向にも力をかける
				vec.y = 5.0f;

				// 吹っ飛ばす
				player.AddImpulse(vec);
			}
		}
	}
}

// 登場ステートへ遷移
void SikaTentyo::TransitionEntryState()
{
	state = State::Entry;

	model->PlayAnimation(Anim_Entry, false);
}

// 登場ステート更新処理
void SikaTentyo::UpdateEntryState(float elapsedTime)
{
	if (position.y > 1.3f)
	{
		velocity.y -= 15 * elapsedTime;
	}
	else if(position.y <= 1.3f)
	{
		velocity.y = 0;
		position.y = 1.3f;

		//! 開始ステートへ遷移
		TransitionStartState();
	}
}

// 開始ステートへ遷移
void SikaTentyo::TransitionStartState()
{
	state = State::Start;

	model->PlayAnimation(Anim_Start, false);

	stateChangeWaitTimer = 0.0f;

	actionTimer = 0.0f;
}

// 開始ステート更新処理
void SikaTentyo::UpdateStartState(float elapsedTime)
{
	velocity.y = 0;
	position.y = 1.3f;

	if (actionTimer > 3.4f && !effectPlay)
	{
		DirectX::XMFLOAT3 pos = position;
		pos.y += height * 0.5f;

		roarEffect->Play(pos, 1.5f);
		effectPlay = true;
	}
	else
		actionTimer += elapsedTime;

	if (!model->IsPlayAnimation())
	{
		stateChangeWaitTimer += elapsedTime;

		if (stateChangeWaitTimer > 2.0f)
			TransitionMoveState();
	}
}

// 開始ステートへ遷移
void SikaTentyo::TransitionMoveState()
{
	state = State::Move;

	hideSprites = false;

	model->PlayAnimation(Anim_Move, true);
}

// 開始ステート更新処理
void SikaTentyo::UpdateMoveState(float elapsedTime)
{
	HpDirector(10, 50);
	
	// プレイヤーのインスタンス取得
	Player& player = Player::Instance();

	float vx = player.GetPosition().x - position.x;
	float vz = player.GetPosition().z - position.z;

	if (collisionPos.y > 0.0f)
	{
		velocity.y = -6;

		// 移動処理
		Move(vx, vz, 13.0f);
	}
	else if (collisionPos.y <= 0.0f)
	{
		velocity.y = 0;
		position.y = -1.0f;

		dist = vx * vx + vz * vz;
		if(dist > 1000)
			Move(vx, vz, 40.0f);
		else
			// 移動処理
			Move(vx, vz, 3.0f);

		Turn(elapsedTime, vx, vz, 50.0f);
	}
	
	Turn(elapsedTime, vx, vz, 50.0f);
}

// ダメージステートへ遷移
void SikaTentyo::TransitionDamageState()
{
	velocity.y = 10;

	velocity.x = (rand() % 5 + 20) * (rand() % 2 == 1 ? -1 : 1);
	velocity.z = (rand() % 5 + 20) * (rand() % 2 == 1 ? -1 : 1);

	state = State::Damage;

	actionTimer = 1.0f;
}

// ダメージステート更新処理
void SikaTentyo::UpdateDamageState(float elapsedTime)
{
	angle.y += DirectX::XMConvertToRadians(2000) * elapsedTime;

	// ダメージアニメーションが終わったら戦闘待機ステートへ遷移
	if (actionTimer < 0.0f)
	{
		TransitionMoveState();
	}

	actionTimer -= elapsedTime;
}

// 死亡ステートへ遷移
void SikaTentyo::TransitionDeathState()
{
	state = State::Death;

	SceneTitle& scene = SceneTitle::Instance();
	scene.gameClear = true;

	stateTimer = 1.0f;
}

// 死亡ステート更新処理
void SikaTentyo::UpdateDeathState(float elapsedTime)
{
	angle.x += DirectX::XMConvertToRadians(700) * elapsedTime;
	angle.y += DirectX::XMConvertToRadians(700) * elapsedTime;

	velocity.y = 50.0f;

	Player& player = Player::Instance();

	if (!setVelocity)
	{
		if (position.x > player.GetPosition().x)
			velocity.x = 40;
		else
			velocity.x = -40;

		if (position.z > player.GetPosition().z)
			velocity.z = 40;
		else
			velocity.z = -40;

		setVelocity = true;
	}

	// ダメージアニメーションが終わったら自分を破棄
	if (stateTimer <= 0)
	{
		Destroy();
	}

	stateTimer -= elapsedTime;
}

// ダメージ受けた時に呼ばれる
void SikaTentyo::OnDamaged()
{
	invincibleTimer = 1.0f;

	hpShake = true;

	hp -= 20;

	if (hp <= 0)
		// 死亡ステートへ遷移
		TransitionDeathState();
	else
		// ダメージステートへ遷移
		TransitionDamageState();
}

// 死亡しと時に呼ばれる
void SikaTentyo::OnDead()
{
	//Destroy();

	// 死亡ステートへ遷移
	TransitionDeathState();
}
