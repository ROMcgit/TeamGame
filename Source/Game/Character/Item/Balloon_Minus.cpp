#include "Balloon_Minus.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"
#include "Audio/SoundManager.h"

#include <algorithm>

// �R���X�g���N�^
Balloon_Minus::Balloon_Minus()
{
	model = std::make_unique<Model>("Data/Model/3.SoratobuHusenWari/Balloon_Minus/Balloon_Minus.mdl");

	//! �G�t�F�N�g
	effect = std::make_unique<Effect>("Data/Effect/Minus.efk");

	// �ʒuY�̃��Z�b�g
	positionResetY = position.y;

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.008f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.8f;
	height = 1.95f;

	collisionOffset.y = -0.55f;

	TransitionMoveState();
	
	SoundManager::Instance().LoadSound("�j��", "Data/Audio/Sound/Burst.wav");
}

// �f�X�g���N�^
Balloon_Minus::~Balloon_Minus()
{
	//delete model;
}

// �X�V����
void Balloon_Minus::Update(float elapsedTime)
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
void Balloon_Minus::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	targetPosition = Player3_SoratobuHusenWari::Instance().GetPosition();
	float vz = abs(targetPosition.z - position.z);
	if (vz < 80)
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// �f�o�b�O�v���~�e�B�u�`��
void Balloon_Minus::DrawDebugPrimitive()
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

void Balloon_Minus::DrawDebugGUI()
{
	if (ImGui::TreeNode("Balloon_Minus"))
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
void Balloon_Minus::TransitionMoveState()
{
	state = State::Move;
}

// �ړ��X�e�[�g�X�V����
void Balloon_Minus::UpdateMoveState(float elapsedTime)
{
	if (G3_SoratobuHusenWari::gameTimer < 30.0f)
		velocity.z = -8.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
		velocity.z = -13.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
		velocity.z = -18.0f;
	else
		velocity.z = -23.0f;

	float posY = position.y == positionResetY ? positionResetY + 0.1f : positionResetY;
	SetPositionYChange(posY, 0.5f);

	//! �v���C���[�Ɠ���������
	if (CollisionVsPlayer3())
		TransitionBreakState();

	if (position.z < -20.0f)
		Destroy();
}

// ���S�X�e�[�g�֑J��
void Balloon_Minus::TransitionBreakState()
{
	state = State::Break;

	DirectX::XMFLOAT3 e = position;
	e.y += height * 0.5f;

	effect->Play(e, { 1.2f, 1.2f, 1.2f });

	SoundManager::Instance().PlaySound("�j��");

	G3_SoratobuHusenWari::score -= 3;
}

// ���S�X�e�[�g�X�V����
void Balloon_Minus::UpdateBreakState(float elapsedTime)
{
	Destroy();
}

// �_���[�W�󂯂����ɌĂ΂��
void Balloon_Minus::OnDamaged()
{
}

// ���S�������ɌĂ΂��
void Balloon_Minus::OnDead()
{
}