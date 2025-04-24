#include "Balloon_Plus.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <algorithm>

// �R���X�g���N�^
Balloon_Plus::Balloon_Plus()
{
	model = std::make_unique<Model>("Data/Model/3.SoratobuHusenWari/Balloon_Plus/Balloon_Plus.mdl");

	// �ʒuY�̃��Z�b�g
	positionResetY = position.y;

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.015f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 1.45f;
	height = 3.55f;

	collisionOffset.y = -0.95f;

	TransitionMoveState();
}

// �f�X�g���N�^
Balloon_Plus::~Balloon_Plus()
{
	//delete model;
}

// �X�V����
void Balloon_Plus::Update(float elapsedTime)
{
	// �X�e�[�g���̍X�V����
	switch (state)
	{
	// �ړ�
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	// ����
	case State::Break:
		UpdateBreakState(elapsedTime);
		break;
	}

	// �L�����N�^�[�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���A�j���[�V�����X�V
	model->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void Balloon_Plus::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player3_SoratobuHusenWari::Instance().GetPosition();
	float vz = abs(targetPosition.z - position.z);
	if (vz < 80)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// �f�o�b�O�v���~�e�B�u�`��
void Balloon_Plus::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Item::DrawDebugPrimitive();

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

void Balloon_Plus::DrawDebugGUI()
{
	if (ImGui::TreeNode("Balloon_Plus"))
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

			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
		}

		ImGui::TreePop();
	}
}

// �ړ��X�e�[�g�֑J��
void Balloon_Plus::TransitionMoveState()
{
	state = State::Move;
}

// �ړ��X�e�[�g�X�V����
void Balloon_Plus::UpdateMoveState(float elapsedTime)
{
	velocity.z = -8.0f;

	float posY = position.y == positionResetY ? positionResetY + 0.1f : positionResetY;
	SetPositionYChange(posY, 0.5f);

	//! �v���C���[�Ɠ���������
	if (CollisionVsPlayer3() || position.z < -10.0f)
		TransitionBreakState();
}

// ���S�X�e�[�g�֑J��
void Balloon_Plus::TransitionBreakState()
{
	state = State::Break;

	G3_SoratobuHusenWari::score += 5;
}

// ���S�X�e�[�g�X�V����
void Balloon_Plus::UpdateBreakState(float elapsedTime)
{
	Destroy();
}

// �_���[�W�󂯂����ɌĂ΂��
void Balloon_Plus::OnDamaged()
{
}

// ���S�������ɌĂ΂��
void Balloon_Plus::OnDead()
{
}