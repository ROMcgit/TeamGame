#include "CameraTarget.h"
#include <imgui.h>
#include "Game/Camera/CameraController.h"

// �R���X�g���N�^
CameraTarget::CameraTarget(DirectorManager* manager)
	: Director(manager)
{
	model = std::make_unique<Model>("Data/Model/CameraTarget/CameraTarget.mdl");

	// �X�P�[��
	scale.x = scale.y = scale.z = 0.01f;

	// �����蔻��
	radius = 0.0f;

	// �A�j���[�V����
	model->PlayAnimation(0, true);
}

// �f�X�g���N�^
CameraTarget::~CameraTarget()
{
}

// �X�V����
void CameraTarget::Update(float elapsedTime)
{
	if(!CameraController::noMoveDebugCamera)
		// ���o�̏�ԍX�V����
		UpdateDirectorState(elapsedTime, true, true, 2.0f);
	else
		// ���o�̏�ԍX�V����
		UpdateDirectorState(elapsedTime, true, false);

	// �^�[�Q�b�g�̈ʒu�̏�ԍX�V����
	UpdateTargetState();

	// ���f���s��X�V
	model->UpdateTransform(transform);

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);
}

// �`�揈��
void CameraTarget::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	if(opacity != 0.0f)
		shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// �f�o�b�O�pGUI�`��
void CameraTarget::DrawDebugGUI()
{
	if (ImGui::TreeNode("CameraTarget"))
	{
		// �g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform"))
		{
			// �ʒu
			ImGui::DragFloat3("Position", &position.x, 0.01f);
			// �����蔻��̈ʒu
			ImGui::DragFloat3("CollisionPos", &collisionPos.x, 0.01f);
			// ��]
			ImGui::DragFloat3("Direction", &direction.x, 0.01f);
			// �X�P�[��
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		}

		ImGui::TreePop();
	}
}

// ��Ԑݒ�
void CameraTarget::SetState(const DirectX::XMFLOAT3 direction,
	const DirectX::XMFLOAT3& position,
	float moveSpeed)
{
	{
		this->direction = direction;
		this->position = position;
		this->moveSpeed = moveSpeed;

		UpdateTransform();
	}
}

// �^�[�Q�b�g�̈ʒu�̏�ԍX�V����
bool CameraTarget::UpdateTargetState()
{
	//! �ʒu��ݒ�
	position = CameraController::target;

	//! �f�o�b�O�J��������Ȃ��Ȃ�A�����ŏ������Ƃ߂�
	if (!CameraController::debugCamera) 
	{
		//! �s�����x��ύX
		SetOpacityChange(0.0f, 2.0f);
		return false;
	}
	else if(opacity == 0.0f)
		//! �s�����x��ύX
		SetOpacityChange(1.0f, 2.0f);

	//! �f�o�b�O�J�����ňړ��ł���Ȃ�
	if(!CameraController::noMoveDebugCamera)
	{
		//! �s�����x��ύX
		SetOpacityChange(1.0f, 2.0f);

		//! �G�~�b�V�u�̋�����ύX
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 1), 0.0f, 2.0f);
	}
	else if(CameraController::noMoveDebugCamera)
	{
		//! �s�����x��ύX
		SetOpacityChange(0.6f, 2.0f);

		//! �G�~�b�V�u�̋�����ύX
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 1), 0.5f, 2.0f);
	}

	return true;
}