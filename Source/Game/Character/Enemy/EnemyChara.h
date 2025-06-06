#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Effect/Effect.h"

// おっさん
class EnemyChara : public Enemy
{
public:
	EnemyChara();
	~EnemyChara() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HPなどの描画
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	// デバッグGUI
	void DrawDebugGUI() override;

protected:
	//ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// ムービー上昇ステートへ遷移
	void TransitionMovieUpState();

	// ムービー上昇ステート更新処理
	void UpdateMovieUpState(float elapsedTime);

	// ムービーダウンステートへ遷移
	void TransitionMovieDownState();

	// ムービーダウンステート更新処理
	void UpdateMovieDownState(float elapsedTime);

	// 上昇ステートへ遷移
	void TransitionUpState();

	// 上昇ステート更新処理
	void UpdateUpState(float elapsedTime);

	// 下降ステートへ遷移
	void TransitionDownState();

	// 下降ステートへ遷移
	void UpdateDownState(float elapsedTime);

	// ダメージステートへ遷移
	void TransitionDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		MovieUp,
		MovieDown,
		Up,
		Down,
		Damage,
	};

	State state = State::MovieUp;

	// アニメーション
	enum Animation
	{
		Anim_Wait,
		Anim_Damage,
	};

private:
	std::unique_ptr<Model> model;

	std::unique_ptr<Effect> effectMinus;
	std::unique_ptr<Effect> effectPlus;

	int modelNum = 0;
};