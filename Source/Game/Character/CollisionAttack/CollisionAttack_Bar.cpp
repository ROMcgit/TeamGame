#include "CollisionAttack_Bar.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Character/Player/Player5_AsibaWatari.h"

#include <imgui.h>

// �R���X�g���N�^
CollisionAttack_Bar::CollisionAttack_Bar(CollisionAttackManager* manager)
	: CollisionAttack(manager)
{
	model = std::make_unique<Model>("Data/Model/5.Asibawatari/Bar.mdl");

	// �p�x
	angle.x = DirectX::XMConvertToRadians(180);

	// �傫��
	scale.x = scale.z = 0.01f;
	scale.y = 0.04f;

	// �����蔻��
	radius = 0.0f;

	// �����蔻��̒���
	collisionOffset = { 0.0f, 0.0f, 0.0f };


	materialColor = { 1,0,0 };

	opacity = 0.5f;
}

// �f�X�g���N�^
CollisionAttack_Bar::~CollisionAttack_Bar()
{
}

// �X�V����
void CollisionAttack_Bar::Update(float elapsedTime)
{
	// ��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);

	// �ʒu�X�V����
	UpdatePos();
}

// �`�揈��
void CollisionAttack_Bar::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	if(!Player5_AsibaWatari::Instance().IsGround())
		shader->Draw(dc, model.get(), materialColor, opacity);
}

// �f�o�b�OGUI�`��
void CollisionAttack_Bar::DrawDebugGUI()
{
	if (ImGui::TreeNode("Bar"))
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

// �ʒu�X�V����
void CollisionAttack_Bar::UpdatePos()
{
	Player5_AsibaWatari& player = Player5_AsibaWatari::Instance();
	position.x = player.GetPosition().x;
	position.y = player.GetPosition().y - 4.0f;
	position.z = player.GetPosition().z;
}
