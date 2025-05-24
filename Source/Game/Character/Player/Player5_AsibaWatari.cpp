#include <imgui.h>
#include "Player5_AsibaWatari.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Camera/CameraController.h"
#include "Game/Character/CollisionAttack/CollisionAttack_Bar.h"
#include "Audio/SoundManager.h"

static Player5_AsibaWatari* instance = nullptr;

// �C���X�^���X�擾
Player5_AsibaWatari& Player5_AsibaWatari::Instance()
{
	return *instance;
}

Player5_AsibaWatari::Player5_AsibaWatari()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	// ���f���ǂݍ���
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	// �d��
	gravity = 0.45f;

	// �o�[�𐶐�
	std::unique_ptr<CollisionAttack_Bar> bar = std::make_unique<CollisionAttack_Bar>(&collisionAttackManager);
	collisionAttackManager.Register(std::move(bar));

	// �����蔻��
	radius = 2.3f;
	height = 15.6f;

	SoundManager& sound = SoundManager::Instance();
	sound.LoadSound("�W�����v", "Data/Audio/Sound/Jump.wav");

	// �ҋ@�X�e�[�g�֑J��
	TransitionWaitState();
}

// �f�X�g���N�^
Player5_AsibaWatari::~Player5_AsibaWatari()
{
}

// �X�V����
void Player5_AsibaWatari::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

	// ���[�r�[���Ȃ�ҋ@�X�e�[�g�֑J��
	if (movieScene)
	{
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

	// �ՓˍU���̍X�V����
	collisionAttackManager.Update(elapsedTime);

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	// ���f���X�V����
	model->UpdateTransform(transform);
}

// �`�揈��
void Player5_AsibaWatari::Render(ID3D11DeviceContext* dc, Shader* shader, bool shadowMap)
{
	shader->Draw(dc, model.get());

	if(!shadowMap)
		collisionAttackManager.Render(dc, shader);
}

// HP�Ȃǂ�UI�`��
void Player5_AsibaWatari::SpriteRender(ID3D11DeviceContext* dc)
{
}

// �ړ����͏���
bool Player5_AsibaWatari::InputMove(float elapsedTime)
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
bool Player5_AsibaWatari::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	{
		if ((gamePad.GetButtonHeld() & GamePad::BTN_A) || (gamePad.GetButtonHeld() & GamePad::BTN_SPACE)) //Z�L�[
		{
			// �W�����v���͂���
			return true;

		}
		return false;
	}
}

// �ҋ@�X�e�[�g�֑J��
void Player5_AsibaWatari::TransitionWaitState()
{
	state = State::Wait;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player5_AsibaWatari::UpdateWaitState(float elapsedTime)
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
	GamePadButton button = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y | GamePad::BTN_SPACE;
	//! �W�����v����
	if (gamePad.GetButtonDown() & button)
	{
		InputJump();
		TransitionJumpState();
	}

	InputMove(elapsedTime);
}

// �ړ��X�e�[�g�֑J��
void Player5_AsibaWatari::TransitionMoveState()
{
	state = State::Move;

	// ����A�j���[�V�����Đ�
	if (model->GetAnimationNum() != Anim_Move)
		model->PlayAnimation(Anim_Move, true);
}

// �ړ��X�e�[�g�X�V����
void Player5_AsibaWatari::UpdateMoveState(float elapsedTime)
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

	GamePadButton button = GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y | GamePad::BTN_SPACE;
	//! �W�����v����
	if (gamePad.GetButtonDown() & button)
	{
		InputJump();
		TransitionJumpState();
	}

	

	InputMove(elapsedTime);
}

// �W�����v�X�e�[�g�֑J��
void Player5_AsibaWatari::TransitionJumpState()
{
	state = State::Jump;

	SoundManager::Instance().PlaySound("�W�����v");

	model->PlayAnimation(Anim_Jump, false);
}

// �W�����v�X�e�[�g�X�V����
void Player5_AsibaWatari::UpdateJumpState(float elapsedTime)
{
	if (isGround)
		TransitionWaitState();

	// �ړ����͏���
	InputMove(elapsedTime);
}

// �_���[�W�X�e�[�g�֑J��
void Player5_AsibaWatari::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damage, false);
}

// �_���[�W�X�e�[�g�X�V����
void Player5_AsibaWatari::UpdateDamageState(float elapsedTime)
{
	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionWaitState();
	}
}

// ���S�X�e�[�g�֑J��
void Player5_AsibaWatari::TransitionDeathState()
{
	state = State::Death;

	// ���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void Player5_AsibaWatari::UpdateDeathState(float elapsedTimae)
{
}

//�f�o�b�O�v���~�e�B�u�`��
void Player5_AsibaWatari::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifdef _DEBUG
	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(collisionPos, radius, height, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// ���n�������ɌĂ΂��
void Player5_AsibaWatari::OnLanding()
{
}

// �_���[�W���󂯂����ɌĂ΂��
void Player5_AsibaWatari::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player5_AsibaWatari::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �����蔻��`��
void Player5_AsibaWatari::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player5_AsibaWatari"))
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