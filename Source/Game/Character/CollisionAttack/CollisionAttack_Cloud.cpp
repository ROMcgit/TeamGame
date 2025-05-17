#include "CollisionAttack_Cloud.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Audio/SoundManager.h"

#include <imgui.h>

// �R���X�g���N�^
CollisionAttack_Cloud::CollisionAttack_Cloud(CollisionAttackManager* manager)
	: CollisionAttack(manager)
{
	model = std::make_unique<Model>("Data/Model/3.SoratobuHusenWari/Cloud/Cloud.mdl");

	// �p�x
	angle.x = DirectX::XMConvertToRadians(180);

	// �傫��
	scale.x = scale.y = scale.z = 0.005f;
	scaleReset = scale;

	collisionType = CollisionType::Box;

	width  = 3.85f;
	height = 2.3f;
	depth  = 1.4f;

	// �}�e���A���̐F
	materialColor.x = materialColor.y = materialColor.z = 0.3f;

	// �����蔻��̒���
	collisionOffset = { 0.0f, 0.0f, 0.0f };
}

// �f�X�g���N�^
CollisionAttack_Cloud::~CollisionAttack_Cloud()
{
}

// �X�V����
void CollisionAttack_Cloud::Update(float elapsedTime)
{
	if (G3_SoratobuHusenWari::gameTimer < 30.0f)
		velocity.z = -8.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
		velocity.z = -12.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
		velocity.z = -16.0f;
	else
		velocity.z = -20.0f;

	//position.z = Player3_SoratobuHusenWari::Instance().GetPosition().z + 3;
	if (Player3_SoratobuHusenWari::Instance().GetPosition().z - 0.5f > position.z)
		SetOpacityChange(0.5f, 0.3f);

	if (position.z < -18.0f)
		Destroy();

	// ��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ��ԍX�V����
	UpdateState();

	// �v���C���[�Ƃ̐ڐG����
	CollisionVsPlayer();

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void CollisionAttack_Cloud::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);
}

// �f�o�b�OGUI�`��
void CollisionAttack_Cloud::DrawDebugGUI()
{
	if (ImGui::TreeNode("Cloud"))
	{
		// �g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform_CollisionAttack"))
		{
#if 1
			// �ʒu
			ImGui::DragFloat3("Position", &position.x, 0.01f);
			// �����蔻��̈ʒu
			ImGui::DragFloat3("CollisionPos", &collisionPos.x, 0.01f);
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
#endif
		}

		// �����蔻��
		if (ImGui::CollapsingHeader("Collision"))
		{
#if 1
			static const char* items[] = { "Box", "Sphere", "Cylinder" };
			int currentItem = static_cast<int>(collisionType);

			if (ImGui::Combo("Collision Type", &currentItem, items, static_cast<int>(CollisionType::Count))) {
				collisionType = static_cast<CollisionType>(currentItem);
			}

			switch (collisionType)
			{
			case CollisionType::Box:
			{
				// ��
				ImGui::DragFloat("Width", &width, 0.01f);
				// ����
				ImGui::DragFloat("Height", &height, 0.01f);
				// ���s
				ImGui::DragFloat("Depth", &depth, 0.01f);
			}
			break;
			case CollisionType::Sphere:
			{
				// ���a
				ImGui::DragFloat("Radius", &radius, 0.01f);
			}
			break;
			case CollisionType::Cylinder:
			{
				// ���a
				ImGui::DragFloat("Radius", &radius, 0.01f);
				// ����
				ImGui::DragFloat("Height", &height, 0.01f);
			}
			break;
			default:
				break;
			}

			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-------------------------------------------------------------------------------------------------------//
						// �f�o�b�O�v���~�e�B�u�`��̐F
			ImGui::ColorEdit3("DebugPrimitiveColor", &debugPrimitiveColor.x);
			// �f�o�b�O�v���~�e�B�u�`��̐F�̐��l
			ImGui::InputFloat3("Color", &debugPrimitiveColor.x);
			//-------------------------------------------------------------------------------------------------------//
			ImGui::Spacing(); // ��s�󂯂�
			ImGui::Separator(); // �Z�N�V�����̊Ԃɋ�؂����\��
			ImGui::Spacing(); // ��s�󂯂�
			//-------------------------------------------------------------------------------------------------------//
						// �����蔻����ǂꂭ�炢���炷��
			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
#endif
		}

		ImGui::TreePop();
	}
}

// �G�~�V�b�u�̍X�V����
void CollisionAttack_Cloud::UpdateState()
{
	float scale = this->scale.x == scaleReset.x ? scaleReset.x + 0.002f : scaleReset.x;
	//! �傫����ύX����
	SetScaleAllChange(DirectX::XMFLOAT3(scale, scale, scale), 0.2f);

	float emissive = emissiveStrength == 0.0f ? 0.7f : 0.0f;
	//! �G�~�V�b�u�̋�����ύX
	SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 1, 1), emissive, 0.2f);
}

// �v���C���[�Ƃ̐ڐG
void CollisionAttack_Cloud::CollisionVsPlayer()
{
	Player3_SoratobuHusenWari& player = Player3_SoratobuHusenWari::Instance();

	//! �v���C���[�ƐڐG������
	if (CollisionVsPlayer3() && !player.isDamage)
	{
		player.isDamage = true;
	}
}