#include "GS2_Sundome.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_OniGokko.h"
#include "Other/Collision.h"
#include "Game/Camera/CameraController.h"
#include "Game/Scene/SceneGameSelect.h"

// �R���X�g���N�^
GS2_Sundome::GS2_Sundome()
{
	model = std::make_unique<Model>("Data/Model/GameSelect/Onigokko.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.05f;

	angle.y = DirectX::XMConvertToRadians(180);

	collisionType = CollisionType::Box;
	// ���A�����ݒ�
	width = 22.55f;
	height = 7.0f;
	depth = 2.5f;

	collisionOffset.y = 0.0f;

	gravity = 0.0f;
}

// �f�X�g���N�^
GS2_Sundome::~GS2_Sundome()
{
	//delete model;
}

// �X�V����
void GS2_Sundome::Update(float elapsedTime)
{
	// �X�e�[�g���̍X�V����
	switch (state)
	{
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	}

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
void GS2_Sundome::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// HP�Ȃǂ̕`��
void GS2_Sundome::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void GS2_Sundome::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	GameSelect::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();
}

// �f�o�b�OGUI
void GS2_Sundome::DrawDebugGUI()
{
	if (ImGui::TreeNode("GS0_Onigokko"))
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

		if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("Width", &width, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
			ImGui::DragFloat("Depth", &depth, 0.01f);
			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
		}

		ImGui::TreePop();
	}
}

// �꒣��ݒ�
void GS2_Sundome::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// �^�[�Q�b�g�ʒu�������_���ݒ�
void GS2_Sundome::SetRandomTargetPosition()
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
void GS2_Sundome::MoveToTarget(float elapsedTime, float speedRate)
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
void GS2_Sundome::TransitionWaitState()
{
	state = State::Wait;

	// �^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void GS2_Sundome::UpdateWaitState(float elapsedTime)
{
}

// �v���C���[�Ƃ̐ڐG����
void GS2_Sundome::CollisionVsPlayer()
{
	Player0_Onigokko& player = Player0_Onigokko::Instance();

	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectBoxVsCylinder(
		collisionPos,
		width,
		height,
		depth,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		//! �V�[����؂�ւ���
		SceneGameSelect::gameSelect = SceneGameSelect::GameSelect::Sundome;
		SceneGameSelect::sceneChange = true;
		player.SetPosition(outPosition);
	}
}