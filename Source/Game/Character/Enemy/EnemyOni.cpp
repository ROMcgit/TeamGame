#include "EnemyOni.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"
#include "Audio/BgmManager.h"

#include <algorithm>
#include "EnemyDarumasangaKoronda.h"

bool EnemyOni::tracking = false;

// �R���X�g���N�^
EnemyOni::EnemyOni(bool perception)
{
	//! �@�m���Ă��邩
	this->perception = perception;

	model = std::make_unique<Model>("Data/Model/0.Onigokko/Oni/Oni.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.6f;
	height = 5.0f;

	opacity = 0;
	SetOpacityChange(1.0f, 0.8f);

	TransitionEntryState();
}

// �f�X�g���N�^
EnemyOni::~EnemyOni()
{
	//delete model;
}

// �X�V����
void EnemyOni::Update(float elapsedTime)
{
	if (Timer::GetTimeM_Int() <= 0 && Timer::GetTimeS_Int() <= 0 && !deathState)
	{
		TransitionDeathState();
		deathState = true;
	}

	// �X�e�[�g���̍X�V����
	switch (state)
	{
	// �o��
	case State::Entry:
		UpdateEntryState(elapsedTime);
		break;
	// �ҋ@
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	// �ړ�
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	// �΂�
	case State::Laugh:
		UpdateLaughState(elapsedTime);
		break;
	// �ǐ�
	case State::Tracking:
		UpdateTrackingState(elapsedTime);
		break;
	// ����
	case State::Tired:
		UpdateTiredState(elapsedTime);
		break;
	// �U��
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	// ���S
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	// �L�����N�^�[�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// �v���C���[�Ƃ̓����蔻��
	CollisionVsPlayer();

	// ���f���A�j���[�V�����X�V
	model->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void EnemyOni::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if ((dist < 7000 || G0_Onigokko::movieScene) && opacity > 0)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// HP�Ȃǂ̕`��
void EnemyOni::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyOni::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	//Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	debugRender->DrawCylinder(position, searchRange, 2.0f, { 1,1,1,1 });

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

void EnemyOni::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyOni"))
	{
		ImGui::InputFloat("Dist", &dist);
		ImGui::InputFloat3("Velocity", &velocity.x);

		// �g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// �ʒu
			ImGui::DragFloat3("Position", &position.x, 0.01f);
			// ��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::DragFloat3("Angle", &a.x, 0.01f);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// �X�P�[��
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		}
		if (ImGui::CollapsingHeader("Collision"))
		{
			ImGui::DragFloat("Radius", &radius, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
		}

		ImGui::TreePop();
	}
}

// �o��X�e�[�g�֑J��
void EnemyOni::TransitionEntryState()
{
	state = State::Entry;

	stateChangeWaitTimer = 1.0f;

	model->PlayAnimation(Anim_Entry, false);
}

// �o��X�e�[�g�X�V����
void EnemyOni::UpdateEntryState(float elapsedTime)
{
	if(!model->IsPlayAnimation())
		stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
		TransitionWaitState();
}

// �ҋ@�X�e�[�g�֑J��
void EnemyOni::TransitionWaitState()
{
	state = State::Wait;

	stateChangeWaitTimer = 1.0f;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyOni::UpdateWaitState(float elapsedTime)
{
	//! ���[�r�[�V�[���łȂ��Ȃ�A�҂����Ԃ����炷
	if (!G0_Onigokko::movieScene)
		stateChangeWaitTimer -= elapsedTime;

	Player0_Onigokko& player = Player0_Onigokko::Instance();

	targetPosition = player.GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if ((dist < searchRange * searchRange && player.GetInvincibleTimer() <= 0) || tracking)
		//! �Њd�X�e�[�g�֑J��
		TransitionLaughState();
	else if (stateChangeWaitTimer <= 0.0f)
		TransitionMoveState();
}

// �ړ��X�e�[�g�֑J��
void EnemyOni::TransitionMoveState()
{
	state = State::Move;

	stateChangeWaitTimer = 10.0f;

	setMoveTarget = false;

	model->PlayAnimation(Anim_Move, true);
}

// �ړ��X�e�[�g�X�V����
void EnemyOni::UpdateMoveState(float elapsedTime)
{
	if(!setMoveTarget)
	{
		if (!perception)
		{
			moveTarget.x = position.x + (rand() % 200 + 200 * (rand() % 2 == 1 ? -1 : 1));
			moveTarget.z = position.z + (rand() % 200 + 200 * (rand() % 2 == 1 ? -1 : 1));

			float setPosX = 295.0f;
			moveTarget.x = std::clamp(moveTarget.x, -setPosX, setPosX);
			float setPosZ = 287.0f;
			moveTarget.z = std::clamp(moveTarget.z, -setPosZ, setPosZ);
		}
		else
		{
			Player0_Onigokko& player = Player0_Onigokko::Instance();
			moveTarget.x = player.GetPosition().x + (3 * (rand() % 2 == 1 ? -1 : 1));
			moveTarget.z = player.GetPosition().z + (3 * (rand() % 2 == 1 ? -1 : 1));
		}

		setMoveTarget = true;
	}

	// �ړ��ʒu�Ɉړ�
	float moveSpeed = perception ? 10 : 60;

	MoveTarget(elapsedTime, moveSpeed);

	float vxm = moveTarget.x - position.x;
	float vzm = moveTarget.z - position.z;
	float d = vxm * vxm + vzm * vzm;
	if (d < (radius * radius) * 2)
	{
		MoveTarget(elapsedTime, 0);
		// �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
	}

	Player0_Onigokko& player = Player0_Onigokko::Instance();
	targetPosition = player.GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if ((dist < searchRange * searchRange && player.GetInvincibleTimer() <= 0) || tracking)
		//! �Њd�X�e�[�g�֑J��
		TransitionLaughState();
	else if (stateChangeWaitTimer <= 0.0f)
		//! �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
}

// �Њd�X�e�[�g�֑J��
void EnemyOni::TransitionLaughState()
{
	if (!tracking) tracking = true;

	BgmManager::Instance().ChangeBgmStatus("���ɂ�����", 1.0f, 3.0f);

	state = State::Laugh;

	stateChangeWaitTimer = 0.5f;

	velocity.x = velocity.z = 0;

	//! �R���g���X�g
	SetContrastChange(1.5f, 0.5f);

	//! �T�`�����[�V����
	SetSaturationChange(1.0f, 0.5f);

	//! �J���[�t�B���^�[
	SetColorFilterChange(DirectX::XMFLOAT3(3.0f, 1.3f, 1.35f), 0.5f);

	//! �N���}�e�B�b�N�A�x���[�V����
	SetChromaticAberrationChange(0.03f, 1.5f);

	model->PlayAnimation(Anim_Laugh, false);
}

// �Њd�X�e�[�g�X�V����
void EnemyOni::UpdateLaughState(float elapsedTime)
{
	if(!model->IsPlayAnimation())
		stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
		TransitionTrackingState();
}

// �ǐՃX�e�[�g�֑J��
void EnemyOni::TransitionTrackingState()
{
	state = State::Tracking;

	stateChangeWaitTimer = 5.0f;

	model->PlayAnimation(Anim_Tracking, true);
}

// �ǐՃX�e�[�g�X�V����
void EnemyOni::UpdateTrackingState(float elapsedTime)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if (dist < 1500)
		//! �v���C���[�Ɍ������Ĉړ�����
		MoveToTarget(elapsedTime, 5);
	else
		//! �v���C���[�̈ʒu���Ɍ������Ĉړ�����
		//! �v���C���[�Ɍ������Ĉړ�����
		MoveToTarget(elapsedTime, 80);

	stateChangeWaitTimer -= elapsedTime;

	// ��莞�ԂŔ���
	if (stateChangeWaitTimer <= 0.0f)
		TransitionTiredState();
}

// ����X�e�[�g�J��
void EnemyOni::TransitionTiredState()
{
	state = State::Tired;

	BgmManager::Instance().ChangeBgmStatus("���ɂ�����", 1.0f, 1.0f);

	tracking = false;

	//! �|�X�g�G�t�F�N�g�����ɖ߂�
	SetPostEffectStatusResetChange();

	model->PlayAnimation(Anim_Tired, false);
}

// ����X�e�[�g�X�V����
void EnemyOni::UpdateTiredState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
		//! �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
}

// �U���X�e�[�g�֑J��
void EnemyOni::TransitionAttackState()
{
	state = State::Attack;

	G0_Onigokko::movieScene = true;

	stateChangeWaitTimer = 2.2f;

	// �U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack, false);
}

// �U���X�e�[�g�X�V����
void EnemyOni::UpdateAttackState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		position = { 50, 5, 80 };

		//! �|�X�g�G�t�F�N�g�����ɖ߂�
		SetPostEffectStatusResetChange();

		TransitionWaitState();
	}
}

// ���S�X�e�[�g�֑J��
void EnemyOni::TransitionDeathState()
{
	state = State::Death;

	angle.y = DirectX::XMConvertToRadians(180);

	stateChangeWaitTimer = 2.0f;
}

// ���S�X�e�[�g�X�V����
void EnemyOni::UpdateDeathState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (!playAnimation && stateChangeWaitTimer <= 0.0f)
	{
		// �_���[�W�A�j���[�V�����Đ�
		model->PlayAnimation(Anim_Death, false);

		playAnimation = true;
	}

	if (!model->IsPlayAnimation() && playAnimation)
		SetOpacityChange(0.0f, 1.5f);
}

// �_���[�W�󂯂����ɌĂ΂��
void EnemyOni::OnDamaged()
{
}

// ���S���Ǝ��ɌĂ΂��
void EnemyOni::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �v���C���[�Ƃ̓����蔻��
void EnemyOni::CollisionVsPlayer()
{
	Player0_Onigokko& player = Player0_Onigokko::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		position,
		radius,
		height,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition
	) && player.GetInvincibleTimer() <= 0)
	{
		TransitionAttackState();
		player.ApplyDamage(1, 10, 0);
	}
}

// �ړ��ʒu�Ɉړ�
void EnemyOni::MoveTarget(float elapsedTime, float speedRate)
{
	// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = moveTarget.x - position.x;
	float vz = moveTarget.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// �ړ�����
	Move3D(vx, vz, moveSpeed * speedRate);
	Turn3DNotCancel(elapsedTime, vx, vz, turnSpeed * speedRate);
}
