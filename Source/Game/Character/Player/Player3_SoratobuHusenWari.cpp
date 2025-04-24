#include <imgui.h>
#include "Player3_SoratobuHusenWari.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Camera/CameraController.h"

static Player3_SoratobuHusenWari* instance = nullptr;

// �C���X�^���X�擾
Player3_SoratobuHusenWari& Player3_SoratobuHusenWari::Instance()
{
	return *instance;
}

Player3_SoratobuHusenWari::Player3_SoratobuHusenWari()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	// ���f���ǂݍ���
	model = std::make_unique <Model>("Data/Model/3.SoratobuHusenWari/Player/Player.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 1.35f;
	height = 4.4f;

	opacity = 0.8f;

	// �d��
	gravity = 0.3f;

	// �ړ��X�e�[�g�֑J��
	TransitionMoveState();
}

// �f�X�g���N�^
Player3_SoratobuHusenWari::~Player3_SoratobuHusenWari()
{
}

// �X�V����
void Player3_SoratobuHusenWari::Update(float elapsedTime)
{
	position.z = 0.0f;

	if (CameraController::debugCamera) return;

	// ���[�r�[���Ȃ�ҋ@�X�e�[�g�֑J��
	if (movieScene)
	{
		// �S�Ă̒e��j������
		int projectileCount = projectileManager.GetProjectileCount();
		for (int i = 0; i < projectileCount; ++i)
		{
			Projectile* projectile = projectileManager.GetProjectile(i);

			// �e�j��
			projectile->Destroy();
		}

		// ���[�r�[���̃A�j���[�V����
		if (!movieAnimation)
		{
			state = State::Move; // �X�e�[�g��ҋ@�ɕύX
			model->PlayAnimation(movieAnimNum, movieAnimLoop);
			movieAnimation = true;
		}
	}
	// ���[�r�[���ł͖�����
	else
		movieAnimation = false; // ���[�r�[���ɑҋ@�X�e�[�g���ǂ���

	if (!movieScene)
	{
		// �X�e�[�g���̏���
		switch (state)
		{
		case State::Move:
			UpdateMoveState(elapsedTime);
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

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �ՓˍU���X�V����
	collisionAttackManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayer3_SoratobuHusenWariVsEnemies();

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	// ���f���X�V����
	model->UpdateTransform(transform);
}

// �`�揈��
void Player3_SoratobuHusenWari::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity, emissiveColor, emissiveStrength);

	// �e�ە`�揈��
	projectileManager.Render(dc, shader);

	// �ՓˍU���`�揈��
	collisionAttackManager.Render(dc, shader);
}

// HP�Ȃǂ�UI�`��
void Player3_SoratobuHusenWari::SpriteRender(ID3D11DeviceContext* dc)
{
}

// �ړ����͏���
bool Player3_SoratobuHusenWari::InputMove(float elapsedTime)
{
	// �i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// �ړ�����
	Move3D(moveVec.x, moveVec.z, moveSpeed);

	// ���񏈗�
	Turn3D_Player(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	// �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
	return !(moveVec.x == 0.0f && moveVec.z == 0.0f);
}

// �ړ��X�e�[�g�֑J��
void Player3_SoratobuHusenWari::TransitionMoveState()
{
	state = State::Move;

	// ����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Move, true);
}

// �ړ��X�e�[�g�X�V����
void Player3_SoratobuHusenWari::UpdateMoveState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	float move             = 5;
	float angleZ           = 10;
	float angleZChangeTime = 0.2f;
	if ((gamePad.GetButtonHeld() & GamePad::BTN_LEFT) && !(gamePad.GetButtonHeld() & GamePad::BTN_RIGHT))
	{
#if 1
		velocity.x = -move;

		//! �p�x�̕ύX������
		SetAngleChangeUnlock();

		//! �p�xZ��ύX����
		SetAngleZChange(DirectX::XMConvertToRadians(angleZ), angleZChangeTime);
#endif
	}
	else if ((gamePad.GetButtonHeld() & GamePad::BTN_RIGHT) && !(gamePad.GetButtonHeld() & GamePad::BTN_LEFT))
	{
#if 1
		velocity.x = move;

		//! �p�x�̕ύX������
		SetAngleChangeUnlock();

		//! �p�xZ��ύX����
		SetAngleZChange(DirectX::XMConvertToRadians(-angleZ), angleZChangeTime);
#endif
	}
	else
	{
#if 1
		velocity.x = 0;

		//! �p�x�ύX������
		SetAngleChangeUnlock();

		//! �p�xZ��ύX����
		SetAngleZChange(DirectX::XMConvertToRadians(0), angleZChangeTime);
#endif
	}

	GamePadButton button =
	GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;
	if (gamePad.GetButtonHeld() & button)
	{
		velocity.y += 100 * elapsedTime;
	}

	velocity.y = std::clamp(velocity.y, -5.0f, 5.0f);


	if (isDamage)
		TransitionDamageState();
}

// �_���[�W�X�e�[�g�֑J��
void Player3_SoratobuHusenWari::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damage, false);

	//! �J���[�t�B���^�[��ύX����
	SetColorFilterChange(DirectX::XMFLOAT3(Camera::colorFilterReset.x + 1.0f, Camera::colorFilterReset.y, Camera::colorFilterReset.z));

	//! �J�����V�F�C�N
	SetCameraShake(0.2f, DirectX::XMINT3(10, 70, 0));

	stateChangeWaitTimer = 1.5f;
}

// �_���[�W�X�e�[�g�X�V����
void Player3_SoratobuHusenWari::UpdateDamageState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (stateChangeWaitTimer <= 0.0f)
	{
		//! �J���[�t�B���^�[��߂�
		SetColorFilterResetChange();

		//! �ړ��X�e�[�g�ɑJ��
		TransitionMoveState();

		isDamage = false;
	}
}

// ���S�X�e�[�g�֑J��
void Player3_SoratobuHusenWari::TransitionDeathState()
{
	state = State::Death;

	// ���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void Player3_SoratobuHusenWari::UpdateDeathState(float elapsedTimae)
{
}

// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player3_SoratobuHusenWari::CollisionPlayer3_SoratobuHusenWariVsEnemies()
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
		//	Player3_SoratobuHusenWari::GetPosition(),
		//	Player3_SoratobuHusenWari::GetRadius(),
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
			//float diff = Player3_SoratobuHusenWari::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player3_SoratobuHusenWari::Jump(10);
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
void Player3_SoratobuHusenWari::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(collisionPos, radius, height,{ debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// ���n�������ɌĂ΂��
void Player3_SoratobuHusenWari::OnLanding()
{
}

// �_���[�W���󂯂����ɌĂ΂��
void Player3_SoratobuHusenWari::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player3_SoratobuHusenWari::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �����蔻��`��
void Player3_SoratobuHusenWari::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player3_SoratobuHusenWari"))
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