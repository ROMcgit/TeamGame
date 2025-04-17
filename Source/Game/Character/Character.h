#pragma once

#include "GameObjectBase.h"

// キャラクター
class Character : public GameObjectBase, public std::enable_shared_from_this<Character>
{
public:
	Character() {}
	virtual ~Character() {}

	// ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime, float defensesUpTime = 0.0f);

	/**************************************************************************************/

		/*! ゲッター */
#if 1

//---------------------------------------------------------------------------//

	/*! 敵関連 */
#if 1

	// モデルの取得
	virtual Model* GetModel() { return nullptr; }

	// 指定したインデックスのモデルを取得
	virtual Model* GetModels(int modelNum) { return nullptr; }

	// フェードを取得
	virtual Fade* GetFade() { return nullptr; }

	// ターゲットの位置を取得
	const DirectX::XMFLOAT3& GetTargetPosition() const { return targetPosition; }

#endif

	//---------------------------------------------------------------------------//

		/*! マネージャー */
#if 1


#endif

#endif // ゲッター

protected:

	//!\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
		/*! 更新処理など */

	/**************************************************************************************/

		/*! 移動関連 */
#if 1

	// スティック入力値から移動ベクトルを習得(3D)
	DirectX::XMFLOAT3 GetMoveVec() const;

	// スティック入力値から移動ベクトルを習得(2D)
	DirectX::XMFLOAT3 GetMoveVec_2D() const;

	// 移動処理(3D)_プレイヤー
	void Move3D_Player(float vx, float vz, float speed, bool slide = false);

	// 移動処理(3D)
	void Move3D(float vx, float vz, float speed);

	// 移動処理(2D)
	void Move2D_Player(float vx, float speed, bool slide = false);

	// 旋回処理(ゼロベクトルでも処理を止めない)
	void Turn3DNotCancel(float elapsedTime, float vx, float vz, float speed);

	// 旋回処理(3D)
	void Turn3D_Player(float elapsedTime, float vx, float vz, float speed);

	// 旋回処理(2D)
	void Turn2D_Player();

	// ジャンプ処理
	void Jump(float speed);

#endif

	/**************************************************************************************/

public:

	/*! 敵関連 */
#if 1

	// ターゲットに到着したかを判定
	bool IsArrivalTarget();

	// ターゲットの方を向く
	void DirectionToTarget(float elapsedTime, float speedRate);

	// ターゲットの方へ一直線に進む
	void MoveToDirection_3D(float speedRate);

	// 敵の破棄処理
	void Destroy_Enemy();

#endif
};