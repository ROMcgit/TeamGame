#include "EnemyStrongLoading.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"
#include <ProjectileStraight.h>
#include <ProjectilePlayer.h>
#include <Input/GamePad.h>
#include <Input/Input.h>
#include <EnemyManager.h>
#include "WallManager.h"
#include "SceneTitle.h"

// コンストラクタ
EnemyStrongLoading::EnemyStrongLoading()
{
	model = std::make_unique<Model>("Data/Model/Enemy/enemy02.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.001f; //サイズは0.02f

	// 幅、高さ設定
	radius = 0.5f;
	height = 1.4f;

	offset.y = -0.7;

	// 徘徊ステートへ遷移
	TransitionLoadingState();

	health = 5;
}

// デストラクタ
EnemyStrongLoading::~EnemyStrongLoading()
{
	//delete model;
}

// 更新処理
void EnemyStrongLoading::Update(float elapsedTime)
{
	// ステート毎の更新処理
	switch (state)
	{
	case State::Loading:
		UpdateLoadingState();
		break;
	}

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// モデルアニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void EnemyStrongLoading::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
	// 弾丸描画処理
}

// ローディング画面
void EnemyStrongLoading::TransitionLoadingState()
{
	state = State::Loading;

	model->PlayAnimation(0, true);
}

// ローディング画面
void EnemyStrongLoading::UpdateLoadingState()
{
	position.x -= posWave;

	if (position.x < -8.5f)
	{
		posWave += 0.03f;
		position.x = 8.5f;
	}
	if (posWave > 0.3f) posWave = 0.3f;
}