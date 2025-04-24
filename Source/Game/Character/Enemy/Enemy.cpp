#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyManager1.h"
#include "Game/Character/Enemy/EnemyManager2.h"
#include "Game/Character/Enemy/EnemyManager3.h"

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// 衝突判定用のデバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

void Enemy::Destroy1()
{
	EnemyManager1::Instance().Remove(this);
}

void Enemy::Destroy2()
{
	EnemyManager2::Instance().Remove(this);
}

void Enemy::Destroy3()
{
	EnemyManager3::Instance().Remove(this);
}

// ターゲットの方を向く
void Enemy::DirectionToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 向きの処理
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// 目標地点向きながら移動
void Enemy::MoveToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move3D(vx, vz, moveSpeed * speedRate);
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}