#include <imgui.h>
#include "Player0_Onigokko.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"
#include "Game/Scene/G0_Onigokko.h"

static Player0_Onigokko* instance = nullptr;

bool Player0_Onigokko::damage = false;

// �C���X�^���X�擾
Player0_Onigokko& Player0_Onigokko::Instance()
{
	return *instance;
}

Player0_Onigokko::Player0_Onigokko()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	// �_���[�W
	damage = false;

	//! �̗�
	hp = 3;

	for(int i = 0; i < 3; i++)
	{
		std::string filePath = "Data/Sprite/0.Onigokko/Heart" + std::to_string(i + 1) + ".png";

		heart[i] = std::make_unique<Sprite>(filePath.c_str());
	}

	// ���f���ǂݍ���
	model = std::make_unique <Model>("Data/Model/Ai/Ai.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;

	// �d��
	gravity = 0;

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 0.6f;
	height = 5.0f;

	// �ړ����x
	moveSpeed = 5.0f;

	// �ҋ@�X�e�[�g�֑J��
	TransitionWaitState();
}

// �f�X�g���N�^
Player0_Onigokko::~Player0_Onigokko()
{
}

// �X�V����
void Player0_Onigokko::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

	// �ړ����Ă��Ȃ��Ȃ�
	if ((gamePad.GetAxisLX() == 0 && gamePad.GetAxisLY() == 0))
		velocity.x = velocity.z = 0;

	if (invincibleTimer > 0)
		opacity = 0.5f;
	else
		opacity = 1.0f;

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
		case State::Damage:
			UpdateDamageState(elapsedTime);
			break;
		case State::Death:
			UpdateDeathState(elapsedTime);
			break;
		}
	}

	UpdateVelocity(elapsedTime);

	// �L�����N�^�[��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	// ���f���X�V����
	model->UpdateTransform(transform);
}

// �`�揈��
void Player0_Onigokko::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get(), materialColor, opacity);
}

// HP�Ȃǂ�UI�`��
void Player0_Onigokko::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	for(int i = 0; i < hp; i++)
	{
		float textureWidth = static_cast<float>(heart[i]->GetTextureWidth());
		float textureHeight = static_cast<float>(heart[i]->GetTextureHeight());

		//! �n�[�g
		heart[i]->Render(dc,
			0, 0,
			screenWidth, screenHeight,
			0, 0,
			textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
}

// �ړ����͏���
bool Player0_Onigokko::InputMove(float elapsedTime)
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

// �ҋ@�X�e�[�g�֑J��
void Player0_Onigokko::TransitionWaitState()
{
	state = State::Wait;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Wait, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player0_Onigokko::UpdateWaitState(float elapsedTime)
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

	InputMove(elapsedTime);
}

// �ړ��X�e�[�g�֑J��
void Player0_Onigokko::TransitionMoveState()
{
	state = State::Move;

	// ����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Move, true);
}

// �ړ��X�e�[�g�X�V����
void Player0_Onigokko::UpdateMoveState(float elapsedTime)
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
		// �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
	}

	InputMove(elapsedTime);
}

// �_���[�W�X�e�[�g�֑J��
void Player0_Onigokko::TransitionDamageState()
{
	state = State::Damage;

	stateChangeWaitTimer = 2.2f;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damage, false);
}

// �_���[�W�X�e�[�g�X�V����
void Player0_Onigokko::UpdateDamageState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation() && stateChangeWaitTimer <= 0.0f)
	{
		position = { 0, 5, 0 };
		TransitionWaitState();
	}
}

// ���S�X�e�[�g�֑J��
void Player0_Onigokko::TransitionDeathState()
{
	state = State::Death;

	// ���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void Player0_Onigokko::UpdateDeathState(float elapsedTimae)
{
}

//�f�o�b�O�v���~�e�B�u�`��
void Player0_Onigokko::DrawDebugPrimitive()
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
void Player0_Onigokko::OnLanding()
{
}

// �_���[�W���󂯂����ɌĂ΂��
void Player0_Onigokko::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player0_Onigokko::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �����蔻��`��
void Player0_Onigokko::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player0_Onigokko"))
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