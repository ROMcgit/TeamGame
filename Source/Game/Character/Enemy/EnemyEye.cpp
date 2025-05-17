#include "EnemyEye.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"
#include "EnemyOni.h"

#include <algorithm>

// �R���X�g���N�^
EnemyEye::EnemyEye()
{
	model = std::make_unique<Model>("Data/Model/0.Onigokko/Eye/Eye.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.02f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 2.11f;
	height = 1.95f;

	rotationR = rand() % 2 == 1 ? true : false;

	TransitionEntryState();

	verticalMove = rand() % 2 == 1 ? true : false;
}

// �f�X�g���N�^
EnemyEye::~EnemyEye()
{
	//delete model;
}

// �X�V����
void EnemyEye::Update(float elapsedTime)
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
	}

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	// �L�����N�^�[�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// �v���C���[�Ƃ̏Փˏ���
	CollisionVsPlayer();

	// ���f���A�j���[�V�����X�V
	model->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void EnemyEye::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player0_Onigokko::Instance().GetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;
	if (dist < 2000 && !G0_Onigokko::movieScene && !EnemyOni::tracking)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// HP�Ȃǂ̕`��
void EnemyEye::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyEye::DrawDebugPrimitive()
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

void EnemyEye::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyEye"))
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

// �ҋ@�X�e�[�g�֑J��
void EnemyEye::TransitionWaitState()
{
	state = State::Wait;

	stateChangeWaitTimer = 1.0f;
}

// �ҋ@�X�e�[�g�X�V����
void EnemyEye::UpdateWaitState(float elapsedTime)
{
	//! ���ړ�
	if (!verticalMove) velocity.x += moveMinus ? -1 : 1;
	else velocity.z += moveMinus ? -1 : 1;

	velocity.x = std::clamp(velocity.x, -velocityXMax, velocityXMax);
	velocity.z = std::clamp(velocity.z, -velocityZMax, velocityZMax);

	actionTimer += elapsedTime;
	if (actionTimer > 5.0f)
	{
		velocityXMax = rand() % 6;
		velocityZMax = rand() % 6;

		moveMinus = !moveMinus;
		actionTimer = 0.0f;
	}


	//! ���[�r�[�V�[���łȂ��Ȃ�A�҂����Ԃ����炷
	if (!G0_Onigokko::movieScene)
		stateChangeWaitTimer -= elapsedTime;

	Player0_Onigokko& player = Player0_Onigokko::Instance();

	targetPosition = player.GetPosition();

	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	dist = vx * vx + vz * vz;

	angle.y += DirectX::XMConvertToRadians(100) * (rotationR ? 1 : -1) * elapsedTime;

	if (SearchPlayer() && player.GetInvincibleTimer() <= 0)
		EnemyOni::tracking = true;
}
// ���S�X�e�[�g�֑J��
void EnemyEye::TransitionDeathState()
{
	angle.y = DirectX::XMConvertToRadians(180);

	stateChangeWaitTimer = 2.0f;
}

// ���S�X�e�[�g�X�V����
void EnemyEye::UpdateDeathState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (!playAnimation && stateChangeWaitTimer <= 0.0f)
	{

		playAnimation = true;
	}

	if (!model->IsPlayAnimation() && playAnimation)
		SetOpacityChange(0.0f, 1.5f);
}

// �_���[�W�󂯂����ɌĂ΂��
void EnemyEye::OnDamaged()
{
}

// ���S���Ǝ��ɌĂ΂��
void EnemyEye::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �v���C���[�Ƃ̏Փˏ���
void EnemyEye::CollisionVsPlayer()
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
		outPosition) && !EnemyOni::tracking)
	{
		player.SetPosition(outPosition);
	}
}

// �v���C���[��T��
bool EnemyEye::SearchPlayer()
{
	// �v���C���[�Ƃ̍��፷���l������3D�ŋ������������
	const DirectX::XMFLOAT3& playerPosition = Player0_Onigokko::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		// �P�ʃx�N�g����
		vx /= distXZ;
		vz /= distXZ;

		// �����x�N�g����
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		// 2�̃x�N�g���̓��ϒl�őO�㔻��
		float dot = (frontX * vx) + (frontZ * vz);
		float threshold = cosf(DirectX::XMConvertToRadians(30.0f)); // 30�x�����W�A���ɕϊ�
		if (dot > threshold)
		{
			return true;
		}
	}
	return false;
}

// �ړ��ʒu�Ɉړ�
void EnemyEye::MoveTarget(float elapsedTime, float speedRate)
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

// �o��X�e�[�g�֑J��
void EnemyEye::TransitionEntryState()
{
	state = State::Entry;
}

// �o��X�e�[�g�X�V����
void EnemyEye::UpdateEntryState(float elapsedTime)
{
	if (Player0_Onigokko::Instance().GetMovieTime() <= 0.0f)
		TransitionWaitState();
}
