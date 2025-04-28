#include "CollisionAttack_Tataki.h"
#include "Audio/SoundManager.h"
#include "Game/Effect/EffectManager.h"
#include "Game/Character/Player/Player3_SoratobuHusenWari.h"
#include "Other/Collision.h"
#include "Game/Character/Enemy/EnemyManager.h"

#include <imgui.h>

// �R���X�g���N�^
CollisionAttack_Tataki::CollisionAttack_Tataki(CollisionAttackManager* manager)
	: CollisionAttack(manager)
{
	collisionType = CollisionType::Sphere;

	radius = 0.0f;

	// �}�e���A���̐F
	materialColor.x = materialColor.y = materialColor.z = 0.3f;

	// �����蔻��̒���
	collisionOffset = { 0.0f, 0.0f, 0.0f };
}

// �f�X�g���N�^
CollisionAttack_Tataki::~CollisionAttack_Tataki()
{
}

// �X�V����
void CollisionAttack_Tataki::Update(float elapsedTime)
{
	// ��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// �G�Ƃ̓����蔻��
	CollisionVsEnemy();
}

// �`�揈��
void CollisionAttack_Tataki::Render(ID3D11DeviceContext* dc, Shader* shader)
{
}

// �f�o�b�OGUI�`��
void CollisionAttack_Tataki::DrawDebugGUI()
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

// �G�Ƃ̓�����
void CollisionAttack_Tataki::CollisionVsEnemy()
{
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	for(int i = 0; i < enemyCount; i++)
	{
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			collisionPos,
			radius,
			enemy->GetCollisionPos(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition) && radius > 0.0f)
		{
			//! �_���[�W��^����
			enemy->ApplyDamage(1, 0, 0);
		}
	}
}