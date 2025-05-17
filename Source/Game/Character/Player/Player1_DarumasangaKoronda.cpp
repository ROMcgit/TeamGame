#include <imgui.h>
#include "Player1_DarumasangaKoronda.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Camera/CameraController.h"
#include "Audio/SoundManager.h"

bool Player1_DarumasangaKoronda::isDamage = false;

static Player1_DarumasangaKoronda* instance = nullptr;

// �C���X�^���X�擾
Player1_DarumasangaKoronda& Player1_DarumasangaKoronda::Instance()
{
	return *instance;
}

Player1_DarumasangaKoronda::Player1_DarumasangaKoronda()
{
	isDamage = false;

	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	// ���f���ǂݍ���
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	// �d��
	gravity = 0.45f;

	// �����蔻��
	radius = 2.3f;
	height = 15.6f;

	// �ҋ@�X�e�[�g�֑J��
	TransitionWaitState();

	SoundManager& sound = SoundManager::Instance();
	sound.LoadSound("�W�����v", "Data/Audio/Sound/Jump.wav");
	sound.LoadSound("�_�b�V��", "Data/Audio/Sound/Dash.wav");
	sound.LoadSound("�U��", "Data/Audio/Sound/Attack.wav");
}

// �f�X�g���N�^
Player1_DarumasangaKoronda::~Player1_DarumasangaKoronda()
{
}

// �X�V����
void Player1_DarumasangaKoronda::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	//! ��������߂�
	if (position.y < 105.0f)
	{
		position.y = 118.0f;
		position.z = position.z - 20.0f;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

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
			state = State::Wait; // �X�e�[�g��ҋ@�ɕύX
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
		case State::Wait:
			UpdateWaitState(elapsedTime);
			break;
		case State::Move:
			UpdateMoveState(elapsedTime);
			break;
		case State::Dash:
			UpdateDashState(elapsedTime);
			break;
		case State::Jump:
			UpdateJumpState(elapsedTime);
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

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	// ���f���X�V����
	model->UpdateTransform(transform);
}

// �`�揈��
void Player1_DarumasangaKoronda::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());

	// �e�ە`�揈��
	projectileManager.Render(dc, shader);
}

// HP�Ȃǂ�UI�`��
void Player1_DarumasangaKoronda::SpriteRender(ID3D11DeviceContext* dc)
{
}

// �ړ����͏���
bool Player1_DarumasangaKoronda::InputMove(float elapsedTime)
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

// �W�����v���͏���
bool Player1_DarumasangaKoronda::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	{
		if ((gamePad.GetButtonHeld() & GamePad::BTN_A) || (gamePad.GetButtonHeld() & GamePad::BTN_LEFT_TRIGGER)) //Z�L�[
		{
			// �W�����v
			Jump(jumpSpeed);
			jumpCount++;

			// �W�����v���͂���
			return true;

		}
		return false;
	}
}

// �ҋ@�X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionWaitState()
{
	state = State::Wait;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateWaitState(float elapsedTime)
{
	// �ړ����͏���
	// �ړ����͂��ꂽ��ړ��X�e�[�g�֑J��

	const GamePadButton ArrowButton =
		GamePad::BTN_UP |
		GamePad::BTN_LEFT |
		GamePad::BTN_RIGHT |
		GamePad::BTN_DOWN;

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetAxisLX() != 0 || gamePad.GetAxisLY() != 0)
	{
		// �ړ��X�e�[�g�֑J��
		TransitionMoveState();
	}

	//! �W�����v����
	if ((gamePad.GetButtonDown() & GamePad::BTN_A) || (gamePad.GetButtonDown() & GamePad::BTN_LEFT_TRIGGER))
	{
		InputJump();
		TransitionJumpState();
	}

	GamePadButton button = GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	//! �_�b�V������
	if (gamePad.GetButtonDown() & button)
	{
		//! �_�b�V���X�e�[�g�֑J��
		TransitionDashState();
	}

	InputMove(elapsedTime);

	if (isDamage)
	{
		TransitionDamageState();
	}
}

// �ړ��X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionMoveState()
{
	state = State::Move;

	// ����A�j���[�V�����Đ�
	if (model->GetAnimationNum() != Anim_Move)
		model->PlayAnimation(Anim_Move, true);
}

// �ړ��X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateMoveState(float elapsedTime)
{
	// �ړ����͏���
	const GamePadButton ArrowButton =
		GamePad::BTN_UP |
		GamePad::BTN_LEFT |
		GamePad::BTN_RIGHT |
		GamePad::BTN_DOWN;

	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetAxisLX() == 0 && gamePad.GetAxisLY() == 0)
	{
		velocity.x = velocity.z = 0;

		// �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
	}

	//! �W�����v����
	if (gamePad.GetButtonDown() & GamePad::BTN_A || (gamePad.GetButtonDown() & GamePad::BTN_LEFT_TRIGGER))
	{
		InputJump();
		TransitionJumpState();
	}

	GamePadButton button = GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	//! �_�b�V������
	if (gamePad.GetButtonDown() & button)
	{
		//! �_�b�V���X�e�[�g�֑J��
		TransitionDashState();
	}

	InputMove(elapsedTime);

	if (isDamage)
	{
		TransitionDamageState();
	}
}

// �W�����v�X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionJumpState()
{
	state = State::Jump;

	SoundManager::Instance().PlaySound("�W�����v");

	model->PlayAnimation(Anim_Jump, false);
}

// �W�����v�X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateJumpState(float elapsedTime)
{
	if (isGround)
		TransitionWaitState();

	// �ړ����͏���
	InputMove(elapsedTime);

	if (isDamage)
	{
		TransitionDamageState();
	}
}

// �_�b�V���X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionDashState()
{
	state = State::Dash;

	stateChangeWaitTimer = 1.0f;

	SoundManager::Instance().PlaySound("�_�b�V��");

	// ����A�j���[�V�����Đ�
	if(model->GetAnimationNum() != Anim_Move)
		model->PlayAnimation(Anim_Move, true);
}

// �_�b�V���X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateDashState(float elapsedTime)
{
	// �O����
	DirectX::XMFLOAT3 dir;

	dir.x = transform._31;
	dir.y = transform._32;
	dir.z = transform._33;

	DirectX::XMVECTOR DIR;
	DIR = DirectX::XMLoadFloat3(&dir);
	DIR = DirectX::XMVector3Normalize(DIR);
	DirectX::XMStoreFloat3(&dir, DIR);

	// �ړ�����
	Move3D(dir.x, dir.z, 30.0f);

	GamePad& gamePad = Input::Instance().GetGamePad();

	//! �W�����v����
	if (gamePad.GetButtonDown() & GamePad::BTN_A || (gamePad.GetButtonDown() & GamePad::BTN_LEFT_TRIGGER))
	{
		InputJump();
		TransitionJumpState();
	}

	stateChangeWaitTimer -= elapsedTime;

	//! �҂����Ԃ�0�Ȃ�
	if (stateChangeWaitTimer <= 0.0f)
	{
		//! �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
	}

	if (isDamage)
	{
		TransitionDamageState();
	}
}

// �_���[�W�X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionDamageState()
{
	isDamage = false;

	velocity.x = velocity.z = 0.0f;

	setVelocityY = false;

	stateChangeWaitTimer = 3.0f;

	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damage, false);
}

// �_���[�W�X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateDamageState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (stateChangeWaitTimer <= 2.2f && stateChangeWaitTimer > 0.0f)
	{
		if(!setVelocityY)
		{
			SoundManager::Instance().PlaySound("�U��");

			setVelocityY = true;
			velocity.y = 20.0f;
		}
		velocity.z = -10.0f;
	}
	else if(stateChangeWaitTimer <= 0.0f)
	{
		
		TransitionWaitState();
	}
}

// ���S�X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionDeathState()
{
	state = State::Death;

	// ���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateDeathState(float elapsedTimae)
{
}

//�f�o�b�O�v���~�e�B�u�`��
void Player1_DarumasangaKoronda::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(collisionPos, radius, height, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// ���n�������ɌĂ΂��
void Player1_DarumasangaKoronda::OnLanding()
{
}

// �_���[�W���󂯂����ɌĂ΂��
void Player1_DarumasangaKoronda::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player1_DarumasangaKoronda::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �����蔻��`��
void Player1_DarumasangaKoronda::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player1_DarumasangaKoronda"))
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