#include "Togemaru.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"

// �R���X�g���N�^
Togemaru::Togemaru()
{
	model = std::make_unique<Model>("Data/Model/Obstacles/Obstacles.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	// ���A�����ݒ�
	radius = 0.5f;

	// �ړ��X�e�[�g�֑J��
	TransitionMoveState();
}

// �f�X�g���N�^
Togemaru::~Togemaru()
{
	//delete model;
}

// �X�V����
void Togemaru::Update(float elapsedTime)
{
	// �X�e�[�g���̍X�V����
	switch (state)
	{
		// �ړ�
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	}

	// �v���C���[�Ƃ̓����蔻��
	CollisionVsPlayer();

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
void Togemaru::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());
}

// HP�Ȃǂ̕`��
void Togemaru::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void Togemaru::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Obstacle::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
}

// �v���C���[�Ƃ̓����蔻��
void Togemaru::CollisionVsPlayer()
{
	Player0_Onigokko& player = Player0_Onigokko::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectSphereVsCylinder(
		position,
		radius,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		
	}
}

// �ړ��X�e�[�g�֑J��
void Togemaru::TransitionMoveState()
{
	state = State::Move;
}

// �ړ��X�e�[�g�X�V����
void Togemaru::UpdateMoveState(float elapsedTime)
{
}