#include <imgui.h>
#include "Player4_OssanTataki.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Tataki.h"
#include "Game/Camera/CameraController.h"
#include "Audio/SoundManager.h"

static Player4_OssanTataki* instance = nullptr;

// �C���X�^���X�擾
Player4_OssanTataki& Player4_OssanTataki::Instance()
{
	return *instance;
}

Player4_OssanTataki::Player4_OssanTataki()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	// ���f���ǂݍ���
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	effect = std::make_unique<Effect>("Data/Effect/Attack.efk");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	debugPrimitiveColor = { 0, 0, 1 };

	//! �ՓˍU���𐶐�
	std::unique_ptr<CollisionAttack_Tataki> tataki = std::make_unique<CollisionAttack_Tataki>(&collisionAttackManager);
	collisionAttackManager.Register(std::move(tataki));

	SoundManager::Instance().LoadSound("�U��", "Data/Audio/Sound/Attack.wav");

	// �ҋ@�X�e�[�g�֑J��
	TransitionWaitState();
}

// �f�X�g���N�^
Player4_OssanTataki::~Player4_OssanTataki()
{
}

// �X�V����
void Player4_OssanTataki::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	if (!movieScene)
	{
		// �X�e�[�g���̏���
		switch (state)
		{
		case State::Wait:
			UpdateWaitState(elapsedTime);
			break;
		case State::Attack:
			UpdateAttackState(elapsedTime);
			break;
		case State::Damage:
			UpdateDamageState(elapsedTime);
			break;
		case State::Death:
			UpdateDeathState(elapsedTime);
			break;
		}
	}

	// �L�����N�^�[��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// �ՓˍU���X�V����
	collisionAttackManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayer4_OssanTatakiVsEnemies();

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	// ���f���X�V����
	model->UpdateTransform(transform);
}

// �`�揈��
void Player4_OssanTataki::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	//shader->Draw(dc, model.get());

	// �ՓˍU���`�揈��
	collisionAttackManager.Render(dc, shader);
}

// HP�Ȃǂ�UI�`��
void Player4_OssanTataki::SpriteRender(ID3D11DeviceContext* dc)
{
}

// �ҋ@�X�e�[�g�֑J��
void Player4_OssanTataki::TransitionWaitState()
{
	state = State::Wait;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player4_OssanTataki::UpdateWaitState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	CollisionAttack* tataki = collisionAttackManager.GetCollisionAttack(0);

	GamePadButton button = GamePad::BTN_UP | GamePad::BTN_DOWN | GamePad::BTN_LEFT | GamePad::BTN_RIGHT;

	int buttonState = gamePad.GetButtonDown();

	DirectX::XMFLOAT3 pos = { 10.0f, 21.5f, -19.0f };

	float offsetX = 32.0f;
	float offsetZ = 17.5f;

	switch (buttonState)
	{
	case GamePad::BTN_UP:    pos.z += offsetZ; break;
	case GamePad::BTN_DOWN:  pos.z -= offsetZ; break;
	case GamePad::BTN_LEFT:  pos.x -= offsetX; break;
	case GamePad::BTN_RIGHT: pos.x += offsetX; break;
	default:
		break;
	}

	tataki->SetPosition(pos);

	if (gamePad.GetButtonDown() & button)
	{
		DirectX::XMFLOAT3 e = pos;

		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for(int i = 0; i < enemyCount; i++)
		{
			std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);
			CollisionAttack* collisionAttack = collisionAttackManager.GetCollisionAttack(0);

			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder
				(collisionAttack->GetPosition(),
					collisionAttack->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				e.y = enemy->GetPosition().y + enemy->GetHeight();
			}
		}

		effect->Play(e, { 1.5f,1.5f,1.5f });

		tataki->SetRadius(3.0f);

		//! �U���X�e�[�g�֑J��
		TransitionAttackState();
	}
}

// �U���X�e�[�g�֑J��
void Player4_OssanTataki::TransitionAttackState()
{
	state = State::Attack;

	SoundManager& sound = SoundManager::Instance();
	sound.StopSound("�U��");
	sound.PlaySound("�U��");

	stateChangeWaitTimer = 0.1f;
}

// �U���X�e�[�g�X�V����
void Player4_OssanTataki::UpdateAttackState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	if (stateChangeWaitTimer <= 0.0f)
	{
		CollisionAttack* tataki = collisionAttackManager.GetCollisionAttack(0);
		tataki->SetRadius(0.0f);

		//! �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
	}
}

// �_���[�W�X�e�[�g�֑J��
void Player4_OssanTataki::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damage, false);
}

// �_���[�W�X�e�[�g�X�V����
void Player4_OssanTataki::UpdateDamageState(float elapsedTime)
{
	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionWaitState();
	}
}

// ���S�X�e�[�g�֑J��
void Player4_OssanTataki::TransitionDeathState()
{
	state = State::Death;

	// ���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void Player4_OssanTataki::UpdateDeathState(float elapsedTimae)
{
}

// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player4_OssanTataki::CollisionPlayer4_OssanTatakiVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

		// �Փˏ���
		DirectX::XMFLOAT3 outPosition;
		//if (Collision::IntersectSphereVsSphere(
		//	Player4_OssanTataki::GetPosition(),
		//	Player4_OssanTataki::GetRadius(),
		//	enemy->GetPosition(),
		//	enemy->GetRadius(),
		//	outPosition
		//))
		//{
		//	// �����o���̌�̈ʒu�ݒ�
		//	enemy->SetPosition(outPosition);
		//}


		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
			//// �v���C���[���G�̏�ɂ��邩�𔻒肷��
			//float diff = Player4_OssanTataki::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player4_OssanTataki::Jump(10);
			//	// ���W�����v�����邽�߂�Y�����̑��x��ݒ肷��
			//}

			//// �����o���̌�̈ʒu�ݒ�
			//enemy->SetPosition(outPosition);

			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			// �ォ�瓥��Â����ꍇ�͏��W�����v����
			if (normal.y > 0.8f)
			{
				// ���W�����v����
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				// �����o����̈ʒu�ݒ�
				enemy->SetPosition(outPosition);
			}

		}

	}
}

//�f�o�b�O�v���~�e�B�u�`��
void Player4_OssanTataki::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(collisionPos, radius, height, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// �ՓˍU���f�o�b�O�v���~�e�B�u�`��
	collisionAttackManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// ���n�������ɌĂ΂��
void Player4_OssanTataki::OnLanding()
{
}

// �_���[�W���󂯂����ɌĂ΂��
void Player4_OssanTataki::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player4_OssanTataki::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �����蔻��`��
void Player4_OssanTataki::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player4_OssanTataki"))
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
			ImGui::DragFloat("Radius", &radius, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
			ImGui::DragFloat3("CollisionOffset", &collisionOffset.x, 0.01f);
		}
		ImGui::TreePop();
	}
}