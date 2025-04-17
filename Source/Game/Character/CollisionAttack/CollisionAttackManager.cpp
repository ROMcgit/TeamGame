#include "CollisionAttackManager.h"

// コンストラクタ
CollisionAttackManager::CollisionAttackManager()
{
}

// デストラクタ
CollisionAttackManager::~CollisionAttackManager()
{
	Clear();
}

// 更新処理
void CollisionAttackManager::Update(float elapsedTime)
{
	// 更新処理
	for (auto& CollisionAttack : collisionAttacks)
	{
		CollisionAttack->Update(elapsedTime);
	}

	collisionAttacks.erase(
		std::remove_if(collisionAttacks.begin(), collisionAttacks.end(),
			[this](const std::unique_ptr<CollisionAttack>& CollisionAttack) {
				return removes.find(CollisionAttack.get()) != removes.end();
			}),
		collisionAttacks.end()
	);

	removes.clear();
}

// 描画処理
void CollisionAttackManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->Render(dc, shader);
	}
}

// 2Dスプライト描画
void CollisionAttackManager::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->SpriteRender(dc, graphics);
	}
}

// デバッグプリミティブ描画
void CollisionAttackManager::DrawDebugPrimitive()
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->DrawDebugPrimitive();
	}
}

// デバッグGUI描画
void CollisionAttackManager::DrawDebugGUI()
{
	for (auto& collisionAttack : collisionAttacks)
	{
		collisionAttack->DrawDebugGUI();
	}
}

// 衝突攻撃登録
void CollisionAttackManager::Register(std::unique_ptr<CollisionAttack> collisionAttack)
{
	collisionAttacks.emplace_back(std::move(collisionAttack));
}

// 衝突攻撃全削除
void CollisionAttackManager::Clear()
{
	collisionAttacks.clear();
}

// 衝突攻撃削除
void CollisionAttackManager::Remove(CollisionAttack* collisionAttack)
{
	// 破棄リストに追加
	removes.insert(collisionAttack);
}