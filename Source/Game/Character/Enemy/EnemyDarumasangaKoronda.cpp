#include "EnemyDarumasangaKoronda.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player1_DarumasangaKoronda.h"
#include "Other/Collision.h"
#include "Game/Scene/G1_DarumasangaKoronda.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <random>
#include <algorithm>

// �R���X�g���N�^
EnemyDarumasangaKoronda::EnemyDarumasangaKoronda()
{
	model = std::make_unique<Model>("Data/Model/0.Onigokko/Oni/Oni.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	gravity = 0.3f;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.6f;
	height = 5.0f;

	opacity = 0;
	SetOpacityChange(1.0f, 0.8f);

	TransitionEntryState();
}

// �f�X�g���N�^
EnemyDarumasangaKoronda::~EnemyDarumasangaKoronda()
{
	//delete model;
}

// �X�V����
void EnemyDarumasangaKoronda::Update(float elapsedTime)
{
	if (Timer::GetTimeM_Int() <= 0 && Timer::GetTimeS_Int() <= 0 && !deathState)
	{
		TransitionDeathState();
		deathState = true;
	}

	if (CollisionVsPlayer() && !deathState)
	{
		//! ���[�r�[�ɂ���
		G1_DarumasangaKoronda::movieScene = true;

		deathState = true;

		//! ���S�X�e�[�g�֑J��
		TransitionDeathState();
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
		// ����
	case State::Look:
		UpdateLookState(elapsedTime);
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
void EnemyDarumasangaKoronda::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player1_DarumasangaKoronda::Instance().GetPosition();
	dist = abs(targetPosition.x - position.x);
	if ((dist < 180 || G1_DarumasangaKoronda::movieScene))
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// HP�Ȃǂ̕`��
void EnemyDarumasangaKoronda::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyDarumasangaKoronda::DrawDebugPrimitive()
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

void EnemyDarumasangaKoronda::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyDarumasangaKoronda"))
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
void EnemyDarumasangaKoronda::TransitionEntryState()
{
	state = State::Entry;

	stateChangeWaitTimer = 1.0f;

	model->PlayAnimation(Anim_Entry, false);
}

// �o��X�e�[�g�X�V����
void EnemyDarumasangaKoronda::UpdateEntryState(float elapsedTime)
{
	if (!G1_DarumasangaKoronda::movieScene)
	{
		stateChangeWaitTimer -= elapsedTime;

		if(stateChangeWaitTimer <= 0.0f)
			TransitionWaitState();
	}
}

// �ҋ@�X�e�[�g�֑J��
void EnemyDarumasangaKoronda::TransitionWaitState()
{
	state = State::Wait;

	//! �p�xY��ύX����
	SetAngleYChange(DirectX::XMConvertToRadians(0), 0.5f);
	
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<> dist(5.0f, 8.0f);
	float time = dist(gen);
	
	//! �X�e�[�g�؂�ւ��܂ł̑҂�����
	stateChangeWaitTimer = time;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyDarumasangaKoronda::UpdateWaitState(float elapsedTime)
{
	//! ���[�r�[�V�[���łȂ��Ȃ�A�҂����Ԃ����炷
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		//! ����X�e�[�g�֑J��
		TransitionLookState();
	}
	else if (stateChangeWaitTimer < 1.3f)
	{
		//! �J���[�t�B���^�[��ύX����
		DirectX::XMFLOAT3 color = Camera::postEffect.colorFilter;
		color.x += 1.5f;
		SetColorFilterChange(color, 1.3f);
	}
}

// ����X�e�[�g
void EnemyDarumasangaKoronda::TransitionLookState()
{
	state = State::Look;

	//! �p�xY��ύX����
	SetAngleYChange(DirectX::XMConvertToRadians(180), 0.3f);

	//! �R���g���X�g
	SetContrast(Camera::postEffect.contrast + 0.05f);

	//! �F����
	SetChromaticAberration(Camera::postEffect.chromaticAberration + 0.05f);

	stateChangeWaitTimer = 1.8f;
}

// ����X�e�[�g�X�V����
void EnemyDarumasangaKoronda::UpdateLookState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		//! �J���[�t�B���^�[��߂�
		SetColorFilterResetChange(0.3f);

		//! �R���g���X�g��߂�
		SetContrastResetChange(0.3f);

		//! �F������߂�
		SetChromaticAberrationResetChange(0.3f);

		//! �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
	}
}

// �U���X�e�[�g�֑J��
void EnemyDarumasangaKoronda::TransitionAttackState()
{
	state = State::Attack;
	
	//! �R���g���X�g
	SetContrastChange(1.5f, 0.5f);

	//! �T�`�����[�V����
	SetSaturationChange(1.0f, 0.5f);

	//! �J���[�t�B���^�[
	SetColorFilterChange(DirectX::XMFLOAT3(3.0f, 1.3f, 1.35f), 0.5f);

	//! �N���}�e�B�b�N�A�x���[�V����
	SetChromaticAberrationChange(0.03f, 1.5f);

	stateChangeWaitTimer = 2.2f;

	// �U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack, false);
}

// �U���X�e�[�g�X�V����
void EnemyDarumasangaKoronda::UpdateAttackState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		//! �|�X�g�G�t�F�N�g�����ɖ߂�
		SetPostEffectStatusResetChange();

		TransitionWaitState();
	}
}

// ���S�X�e�[�g�֑J��
void EnemyDarumasangaKoronda::TransitionDeathState()
{
	state = State::Death;

	angle.y = DirectX::XMConvertToRadians(180);

	stateChangeWaitTimer = 2.0f;
}

// ���S�X�e�[�g�X�V����
void EnemyDarumasangaKoronda::UpdateDeathState(float elapsedTime)
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
void EnemyDarumasangaKoronda::OnDamaged()
{
}

// ���S���Ǝ��ɌĂ΂��
void EnemyDarumasangaKoronda::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �v���C���[�Ƃ̓����蔻��
bool EnemyDarumasangaKoronda::CollisionVsPlayer()
{
	Player1_DarumasangaKoronda& player = Player1_DarumasangaKoronda::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		collisionPos,
		radius,
		height,
		player.GetCollisionPos(),
		player.GetRadius(),
		player.GetHeight(), outPosition))
	{
		return true;
	}

	return false;
}

// �ړ��ʒu�Ɉړ�
void EnemyDarumasangaKoronda::MoveTarget(float elapsedTime, float speedRate)
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
