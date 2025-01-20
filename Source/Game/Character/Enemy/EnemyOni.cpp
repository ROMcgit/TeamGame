#include "EnemyOni.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"

// �R���X�g���N�^
EnemyOni::EnemyOni()
{
	model = std::make_unique<Model>("Data/Model/Slime/Slime.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	// ���A�����ݒ�
	radius = 0.5f;
	height = 1.0f;
}

// �f�X�g���N�^
EnemyOni::~EnemyOni()
{
	//delete model;
}

// �X�V����
void EnemyOni::Update(float elapsedTime)
{
	// �X�e�[�g���̍X�V����
	switch (state)
	{
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	case State::Move:
		UpdateMoveState(elapsedTime);
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	// ���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();

	// �L�����N�^�[�̏�ԍX�V����
	UpdateCharacterState(elapsedTime);

	// ���f���A�j���[�V�����X�V
	model->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void EnemyOni::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HP�Ȃǂ̕`��
void EnemyOni::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyOni::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	// �꒣��͈͂��f�o�b�O�~���`��
	//debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
	//	DirectX::XMFLOAT4(0, 1, 0, 1));

	//// �^�[�Q�b�g�ʒu���f�o�b�O���`��
	//debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	//// ���G�͈͂��f�o�b�O�~���`��
	//debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	//// �U���͈͂��f�o�b�O�~���`��
	//debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

// �꒣��ݒ�
void EnemyOni::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// �^�[�Q�b�g�ʒu�������_���ݒ�
void EnemyOni::SetRandomTargetPosition()
{
	// �꒣��͈͓��Ń����_���Ȉʒu�𐶐�
	float randomX = Mathf::RandomRange(territoryOrigin.x - territoryRange, territoryOrigin.x + territoryRange);
	float randomZ = Mathf::RandomRange(territoryOrigin.z - territoryRange, territoryOrigin.z + territoryRange);

	// Y���W�̓X���C���̍����ɐݒ肷�邩�A�Œ�̒n�ʂ̍����ɐݒ肷��
	float randomY = territoryOrigin.y; // �܂��͌Œ�l�A��: 0.0f

	// �^�[�Q�b�g�ʒu�ɐݒ�
	targetPosition.x = randomX;
	targetPosition.y = randomY;
	targetPosition.z = randomZ;
}

// �ڕW�n�_�ֈړ�
void EnemyOni::MoveToTarget(float elapsedTime, float speedRate)
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

// �ҋ@�X�e�[�g�֑J��
void EnemyOni::TransitionWaitState()
{
	state = State::Wait;

	// �^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyOni::UpdateWaitState(float elapsedTime)
{
}

// �ړ��X�e�[�g�֑J��
void EnemyOni::TransitionMoveState()
{
	state = State::Move;
}

// �ړ��X�e�[�g�X�V����
void EnemyOni::UpdateMoveState(float elapsedTime)
{
}

// �m�[�h�ƃv���C���[�̏Փˏ���
void EnemyOni::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	// �m�[�h�̈ʒu�Ɠ����蔻����s��
	Model::Node* node = model->FindNode(nodeName);
	if (node != nullptr)
	{
		// �m�[�h�̃��[���h���W
		DirectX::XMFLOAT3 nodePosition(
			node->worldTransform._41,
			node->worldTransform._42,
			node->worldTransform._43
		);

		// �����蔻��\��
		Graphics::Instance().GetDebugRenderer()->DrawSphere(
			nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
		);

		// �v���C���[�Ɠ����蔻��
		Player0_Onigokko& player = Player0_Onigokko::Instance();
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition))
		{
		}
	}
}

// �U���X�e�[�g�֑J��
void EnemyOni::TransitionAttackState()
{
	state = State::Attack;

	// �U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack, false);
}

// �U���X�e�[�g�X�V����
void EnemyOni::UpdateAttackState(float elapsedTime)
{
	// �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
	float animationTime = model->GetCurrentAnimationSeconds();
	if (animationTime >= 0.1f && animationTime <= 0.35f)
	{
		// �ڋʃm�[�h�ƃv���C���[�̏Փˏ���
		CollisionNodeVsPlayer("EyeBall", 0.2f);
	}
}

// �_���[�W�X�e�[�g�֑J��
void EnemyOni::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack, false);
}

// �_���[�W�X�e�[�g�X�V����
void EnemyOni::UpdateDamageState(float elapsedTime)
{
}

// ���S�X�e�[�g�֑J��
void EnemyOni::TransitionDeathState()
{
	state = State::Death;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void EnemyOni::UpdateDeathState(float elapsedTime)
{
	// �_���[�W�A�j���[�V�������I������玩����j��
	if (!model->IsPlayAnimation())
	{
		Destroy();
	}
}

// �_���[�W�󂯂����ɌĂ΂��
void EnemyOni::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S���Ǝ��ɌĂ΂��
void EnemyOni::OnDead()
{
	//Destroy();

	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}
