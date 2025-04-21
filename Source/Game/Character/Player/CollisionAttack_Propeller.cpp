#include "CollisionAttack_Propeller.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"

#include <imgui.h>

// �R���X�g���N�^
CollisionAttack_Propeller::CollisionAttack_Propeller(CollisionAttackManager* manager)
	: CollisionAttack(manager)
{
	model = std::make_unique<Model>("Data/Model/1.Character/1.CuppyHunters/1.Boss/0.FrolicOcean/1.Beamman/Projectile/BeamWaveBullet.mdl");

	// �p�x
	angle.x = DirectX::XMConvertToRadians(180);

	// �傫��
	scale.x = scale.y = scale.z = 0.01f;

	// �s�����x
	opacity = 0;
	// �s�����x��ς���
	SetOpacityChange(0.9f, 0.5f);

	// �����蔻��
	radius = 1.2f;

	// �����蔻��̒���
	collisionOffset = { 0.0f, 0.0f, 0.0f };
}

// �f�X�g���N�^
CollisionAttack_Propeller::~CollisionAttack_Propeller()
{
}

// �X�V����
void CollisionAttack_Propeller::Update(float elapsedTime)
{
	// ��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);

	// �ʒu�X�V����
	UpdatePosition(elapsedTime);
}

// �`�揈��
void CollisionAttack_Propeller::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity);
}

// �f�o�b�OGUI�`��
void CollisionAttack_Propeller::DrawDebugGUI()
{
	if (ImGui::TreeNode("Propeller"))
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

// �ړ��X�V����
void CollisionAttack_Propeller::UpdatePosition(float elapsedTime)
{
	Player3_SoratobuHusenWari& player = Player3_SoratobuHusenWari::Instance();

	position = player.GetPosition();
	position.y += player.GetHeight() * 0.7f;
}