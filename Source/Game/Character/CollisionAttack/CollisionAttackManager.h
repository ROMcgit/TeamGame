#pragma once

#include <vector>
#include "CollisionAttack.h"
#include <set>

// 衝突攻撃マネージャー
class CollisionAttackManager
{
public:
	CollisionAttackManager();
	~CollisionAttackManager();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// 2Dスプライト描画
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// デバッグGUI描画
	void DrawDebugGUI();

	// 衝突攻撃登録
	void Register(std::unique_ptr<CollisionAttack> CollisionAttack);

	// 衝突攻撃全削除
	void Clear();

	// 衝突攻撃数取得
	int GetCollisionAttackCount() const { return static_cast<int>(collisionAttacks.size()); }

	// 衝突攻撃取得
	CollisionAttack* GetCollisionAttack(int index) { return collisionAttacks.at(index).get(); }

	// 衝突攻撃削除
	void Remove(CollisionAttack* CollisionAttack);

public:

private:
	std::vector<std::unique_ptr<CollisionAttack>> collisionAttacks;
	std::set<CollisionAttack*> removes;
};