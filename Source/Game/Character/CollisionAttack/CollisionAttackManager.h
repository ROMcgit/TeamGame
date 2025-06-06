#pragma once

#include <vector>
#include "CollisionAttack.h"
#include <set>

// ÕËU}l[W[
class CollisionAttackManager
{
public:
	CollisionAttackManager();
	~CollisionAttackManager();

	// XV
	void Update(float elapsedTime);

	// `æ
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// 2DXvCg`æ
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics);

	// fobOv~eBu`æ
	void DrawDebugPrimitive();

	// fobOGUI`æ
	void DrawDebugGUI();

	// ÕËUo^
	void Register(std::unique_ptr<CollisionAttack> CollisionAttack);

	// ÕËUSí
	void Clear();

	// ÕËUæ¾
	int GetCollisionAttackCount() const { return static_cast<int>(collisionAttacks.size()); }

	// ÕËUæ¾
	CollisionAttack* GetCollisionAttack(int index) { return collisionAttacks.at(index).get(); }

	// ÕËUí
	void Remove(CollisionAttack* CollisionAttack);

public:

private:
	std::vector<std::unique_ptr<CollisionAttack>> collisionAttacks;
	std::set<CollisionAttack*> removes;
};