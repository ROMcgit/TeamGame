#pragma once

#include "Graphics//Model.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Item.h"

// 風船_プラス
class Balloon_Minus : public Item
{
public:
	Balloon_Minus();
	~Balloon_Minus() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	// デバッグGUI
	void DrawDebugGUI() override;

	// 位置Yのリセットの生成
	void SetPositionResetY(float positionResetY) { this->positionResetY = positionResetY; }

protected:
	//ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;

private:

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	// 死亡ステートへ遷移
	void TransitionBreakState();

	// 死亡ステート更新処理
	void UpdateBreakState(float elapsedTime);

private:
	// ステート
	enum class State
	{
		Move,
		Break,
	};

	State state = State::Move;

private:
	std::unique_ptr<Model> model;

	float positionResetY = 0.0f;
};