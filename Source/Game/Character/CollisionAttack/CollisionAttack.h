#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Shader.h"
#include "Game/Effect/Effect.h"
#include "../GameObjectBase.h"
#include <stdexcept>
#include <algorithm>

// 前方宣言
class CollisionAttackManager;

// 衝突攻撃
class CollisionAttack : public GameObjectBase
{
public:
	CollisionAttack(CollisionAttackManager* manager);
	virtual ~CollisionAttack() {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2Dスプライト描画
	virtual void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) {}

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	// デバッグGUI
	virtual void DrawDebugGUI() {}

	// エフェクトのハンドルを取得
	virtual Effekseer::Handle GetHandle() { return -1; }

	// 破棄
	void Destroy();

	//! プレイヤー3との接触処理
	bool CollisionVsPlayer3();

/*****************************************************************************************************************************/
	/*! セッター */
#if 1

	/*! ダメージ関連 */
	void SetOpponentState(int damage, float invincibleTime = 1.5f, float defensesUpTime = 1.0f, const DirectX::XMFLOAT2& knockback = { 10.0f, 6.0f })
	{
		this->opponentDamage         = damage;
		this->opponentInvincibleTime = invincibleTime;
		this->opponentDefensesUpTime = defensesUpTime;
		this->opponentKnockback      = knockback;
	}

#endif

/*****************************************************************************************************************************/
	/*! ゲッター */
#if 1

	/*! ダメージ関連 */

	// プレイヤーなどに与えるダメージを取得
	int GetOpponentDamage() { return opponentDamage; }

	// プレイヤーなどに与える無敵時間を取得
	float GetOpponentInvincibleTime() { return opponentInvincibleTime; }

	// プレイヤーなどに与える防御力アップ時間を取得
	float GetOpponentDefensesUpTime() { return opponentDefensesUpTime; }

	// プレイヤーなどに与えるノックバックを取得
	const DirectX::XMFLOAT2& GetOpponentKnockback() const { return opponentKnockback; }

#endif

public:
	static bool stopCollisionAttack; // 衝突攻撃を止めるか(行列更新処理以外)

protected:
	CollisionAttackManager* manager = nullptr;

	float lifeTimer = 1.0f; // 生存時間

	/*! ダメージ関連 */
#if 1

	int   opponentDamage                = 12;              // プレイヤーなどに与えるダメージ
	float opponentInvincibleTime        = 1.5f;            // プレイヤーなどに与える無敵時間
	float opponentDefensesUpTime        = 1.0f;            // プレイヤーなどに与える防御力アップ時間
	DirectX::XMFLOAT2 opponentKnockback = { 10.0f, 6.0f }; // プレイヤーなどに与えるノックバック

#endif
};