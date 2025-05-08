#pragma once

#include "Graphics//Model.h"
#include "GameSelect.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// 足場渡り
class GS5_AsibaWatari : public GameSelect
{
public:
	GS5_AsibaWatari();
	~GS5_AsibaWatari() override;

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

	// 縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

private:
	// ターゲット位置をランダム設定
	void SetRandomTargetPosition();

	// 目標地点へ移動
	void MoveToTarget(float elapsedTime, float speedRate);

	// 待機ステートへ遷移
	void TransitionWaitState();

	// 待機ステート更新処理
	void UpdateWaitState(float elapsedTime);

	// プレイヤーとの接触処理
	void CollisionVsPlayer();

private:
	// ステート
	enum class State
	{
		Wait,
	};

	// アニメーション
	enum Animation
	{
		Anim_Wait,
	};

private:
	std::unique_ptr<Model> model;

	State state = State::Wait;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};