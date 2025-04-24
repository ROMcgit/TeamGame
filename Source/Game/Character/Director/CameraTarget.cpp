#include "CameraTarget.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Camera/CameraController.h"

// �R���X�g���N�^
CameraTarget::CameraTarget()
{
	model = std::make_unique<Model>("Data/Model/CameraTarget/CameraTarget.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	opacity = 0;

	gravity = 0;

	// ���A�����ݒ�
	radius = 0.5f;
	height = 1.0f;
}

// �f�X�g���N�^
CameraTarget::~CameraTarget()
{
	//delete model;
}

// �X�V����
void CameraTarget::Update(float elapsedTime)
{
	position = CameraController::target;
	if(!CameraController::debugCamera)
		position.y = CameraController::target.y + 17.0f;

	// �X�e�[�g���̍X�V����
	switch (state)
	{
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	}

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	// ���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();

	// �L�����N�^�[�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���A�j���[�V�����X�V
	model->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void CameraTarget::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	if(opacity > 0.2f)
		shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// HP�Ȃǂ̕`��
void CameraTarget::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void CameraTarget::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Director::DrawDebugPrimitive();

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
void CameraTarget::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// �^�[�Q�b�g�ʒu�������_���ݒ�
void CameraTarget::SetRandomTargetPosition()
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
void CameraTarget::MoveToTarget(float elapsedTime, float speedRate)
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
void CameraTarget::TransitionWaitState()
{
	state = State::Wait;

	// �^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void CameraTarget::UpdateWaitState(float elapsedTime)
{
	if (CameraController::debugCamera)
	{
		if (!CameraController::noMoveDebugCamera)
		{
			angle.y += DirectX::XMConvertToRadians(300) * elapsedTime;
			SetOpacityChange(1.0f, 0.5f);
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 1), 0.0f, 0.5f);
		}
		else
		{
			SetOpacityChange(0.5f, 1.0f);
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 1), 0.5f, 0.5f);
		}
	}
	else
		SetOpacityChange(0.0f, 0.5f);
}