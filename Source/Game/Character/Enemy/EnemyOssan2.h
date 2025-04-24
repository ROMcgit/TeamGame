#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// おっさん
class EnemyOssan2 : public Enemy
{
public:
	EnemyOssan2();
	~EnemyOssan2() override;

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
		Up,
		Down,
		Damage,
	};

	State state = State::Up;

	// アニメーション
	enum Animation
	{
		Anim_Entry,
		Anim_Wait,
		Anim_Move,
		Anim_Laugh,
		Anim_Tracking,
		Anim_Tired,
		Anim_Attack,
		Anim_Death
	};

private:
	std::unique_ptr<Model> model;

	int enemyNum = 0;
};