#include "EnemyOssan.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G0_Onigokko.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <algorithm>
#include "EnemyDarumasangaKoronda.h"

#include <random>

// �R���X�g���N�^
EnemyOssan::EnemyOssan()
{
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

	// �㏸�X�e�[�g�֑J��
	TransitionUpState();
}

// �f�X�g���N�^
EnemyOssan::~EnemyOssan()
{
	//delete model;
}

// �X�V����
void EnemyOssan::Update(float elapsedTime)
{
	// �X�e�[�g���̍X�V����
	switch (state)
	{
	// �㏸
	case State::Up:
		UpdateUpState(elapsedTime);
		break;
	case State::Down:
		UpdateDownState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	}

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	// �L�����N�^�[�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���A�j���[�V�����X�V
	model->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void EnemyOssan::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity);
}

// HP�Ȃǂ̕`��
void EnemyOssan::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyOssan::DrawDebugPrimitive()
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

void EnemyOssan::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyOssan"))
	{
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

// �_���[�W�󂯂����ɌĂ΂��
void EnemyOssan::OnDamaged()
{
}

// ���S���Ǝ��ɌĂ΂��
void EnemyOssan::OnDead()
{
}

// �㏸�X�e�[�g�֑J��
void EnemyOssan::TransitionUpState()
{
	state = State::Up;

	//! �ʒuY��ύX����
	SetPositionYChange(21.5f, 1.0f);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0.5f, 2.5f);

	stateChangeWaitTimer = dist(gen);
}

// �㏸�X�e�[�g�X�V����
void EnemyOssan::UpdateUpState(float elapsedTime)
{
	//! �ʒuY�̕ύX���I�������
	if(!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		if (stateChangeWaitTimer <= 0.0f)
		{
			//! ���~�X�e�[�g�֑J��
			TransitionDownState();
		}
	}
}

// ���~�X�e�[�g�֑J��
void EnemyOssan::TransitionDownState()
{
	state = State::Down;
}

// ���~�X�e�[�g�֑J��
void EnemyOssan::UpdateDownState(float elapsedTime)
{

}

// �_���[�W�X�e�[�g�֑J��
void EnemyOssan::TransitionDamageState()
{
	state = State::Damage;
}

// �_���[�W�X�e�[�g�X�V����
void EnemyOssan::UpdateDamageState(float elapsedTime)
{

}