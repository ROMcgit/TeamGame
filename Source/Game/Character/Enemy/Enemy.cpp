#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Game/Character/Enemy/EnemyManager1.h"
#include "Game/Character/Enemy/EnemyManager2.h"
#include "Game/Character/Enemy/EnemyManager3.h"

// �f�o�b�O�v���~�e�B�u�`��
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// �Փ˔���p�̃f�o�b�O����`��
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

// �^�[�Q�b�g�̕�������
void Enemy::DirectionToTarget(float elapsedTime, float speedRate)
{
	// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// �����̏���
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// �ڕW�n�_�����Ȃ���ړ�
void Enemy::MoveToTarget(float elapsedTime, float speedRate)
{
	// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// �ړ�����
	Move3D(vx, vz, moveSpeed * speedRate);
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}