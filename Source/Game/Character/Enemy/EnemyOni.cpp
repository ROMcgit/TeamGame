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

// �ҋ@�X�e�[�g�֑J��
void EnemyOni::TransitionWaitState()
{
	state = State::Wait;

	stateChangeWaitTimer = 0.5f;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyOni::UpdateWaitState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;
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

// �Њd�X�e�[�g�֑J��
void EnemyOni::TransitionThreatState()
{
	state = State::Threat;

	stateChangeWaitTimer = 0.5f;
}

// �Њd�X�e�[�g�X�V����
void EnemyOni::UpdateThreatState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;
}

// �ǐՃX�e�[�g�֑J��
void EnemyOni::TransitionTrackingState()
{
	state = State::Tracking;
}

// �ǐՃX�e�[�g�X�V����
void EnemyOni::UpdateTrackingState(float elapsedTime)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();

	//! �v���C���[�Ɍ������Ĉړ�����
	MoveToTarget(elapsedTime, 100);
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
