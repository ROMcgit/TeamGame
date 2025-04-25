#include "EnemyChara.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Game/Character/Player/Player0_Onigokko.h"
#include "Other/Collision.h"
#include "Game/Scene/G4_OssanTataki.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Timer.h"

#include <algorithm>
#include "EnemyDarumasangaKoronda.h"

#include <random>

// �R���X�g���N�^
EnemyChara::EnemyChara()
{
	models[0] = std::make_unique<Model>("Data/Model/4.OssanTataki/Ossan/Ossan.mdl");
	models[1] = std::make_unique<Model>("Data/Model/4.OssanTataki/Girl/Girl.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.15f;

	gravity = 0;

	angle.y = DirectX::XMConvertToRadians(180);

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 2.9f;
	height = 25.5f;

	opacity = 0;

	// ���[�r�[�㏸�X�e�[�g�֑J��
	TransitionMovieUpState();
}

// �f�X�g���N�^
EnemyChara::~EnemyChara()
{
	//delete model;
}

// �X�V����
void EnemyChara::Update(float elapsedTime)
{
	// �X�e�[�g���̍X�V����
	switch (state)
	{
	// ���[�r�[�㏸
	case State::MovieUp:
		UpdateMovieUpState(elapsedTime);
		break;
	// ���[�r�[���~
	case State::MovieDown:
		UpdateMovieDownState(elapsedTime);
		break;
	// �㏸
	case State::Up:
		UpdateUpState(elapsedTime);
		break;
	// ���~
	case State::Down:
		UpdateDownState(elapsedTime);
		break;
	// �_���[�W
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	}

	// ���͏����X�V
	UpdateVelocity(elapsedTime);

	// �L�����N�^�[�̏�ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���A�j���[�V�����X�V
	models[modelNum]->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	models[modelNum]->UpdateTransform(transform);
}

// �`�揈��
void EnemyChara::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	if(opacity > 0.0f)
	shader->Draw(dc, models[modelNum].get(), materialColor, opacity);
}

// HP�Ȃǂ̕`��
void EnemyChara::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyChara::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRender = Graphics::Instance().GetDebugRenderer();

	// �꒣��͈͂��f�o�b�O�~���`��
	//debugRender->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
	//	DirectX::XMFLOAT4(0, 1, 0, 1));

	//// �^�[�Q�b�g�ʒu���f�o�b�O���`��
	//debugRender->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	//// ���G�͈͂��f�o�b�O�~���`��
	//debugRender->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	//// �U���͈͂��f�o�b�O�~���`��
	//debugRender->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void EnemyChara::DrawDebugGUI()
{
	if (ImGui::TreeNode("EnemyChara"))
	{
		ImGui::InputFloat3("Velocity", &velocity.x);

		ImGui::InputFloat("StateChangeWaitTimer", &stateChangeWaitTimer);

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
		if (ImGui::CollapsingHeader("Collision"))
		{
			ImGui::DragFloat("Radius", &radius, 0.01f);
			ImGui::DragFloat("Height", &height, 0.01f);
		}

		ImGui::TreePop();
	}
}

// �_���[�W�󂯂����ɌĂ΂��
void EnemyChara::OnDamaged()
{
}

// ���S���Ǝ��ɌĂ΂��
void EnemyChara::OnDead()
{
}

// ���[�r�[�㏸�X�e�[�g�֑J��
void EnemyChara::TransitionMovieUpState()
{
	state = State::MovieUp;

	//! �ʒuY��ύX����
	SetPositionYChange(21.5f, 1.0f);

	//! �s�����x��ύX����
	SetOpacityChange(1.0f, 0.5f);

	stateChangeWaitTimer = 2.0f;
}

// ���[�r�[�㏸�X�e�[�g�X�V����
void EnemyChara::UpdateMovieUpState(float elapsedTime)
{
	if (!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		//! �X�e�[�g�ύX�̑҂����Ԃ��I�������
		if (stateChangeWaitTimer <= 0.0f)
			TransitionMovieDownState();
	}
}

// ���[�r�[�_�E���X�e�[�g�֑J��
void EnemyChara::TransitionMovieDownState()
{
	state = State::MovieDown;

	//! �ʒuY��ύX����
	SetPositionYChange(-4.5f, 1.0f);

	//! �s�����x��ύX����
	SetOpacityChange(0.0f, 0.5f);

	stateChangeWaitTimer = 4.0f;
}

// ���[�r�[�_�E���X�e�[�g�X�V����
void EnemyChara::UpdateMovieDownState(float elapsedTime)
{
	if (!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		//! �X�e�[�g�ύX�̑҂����Ԃ��I�������
		if (stateChangeWaitTimer <= 0.0f)
			TransitionUpState();
	}
}

// �㏸�X�e�[�g�֑J��
void EnemyChara::TransitionUpState()
{
	state = State::Up;

	//! �ʒuY��ύX����
	SetPositionYChange(21.5f, 1.0f);

	//! �s�����x��ύX����
	SetOpacityChange(1.0f, 0.5f);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1.3f, 3.0f);

	stateChangeWaitTimer = dist(gen);
}

// �㏸�X�e�[�g�X�V����
void EnemyChara::UpdateUpState(float elapsedTime)
{
	//! �ʒuY�̕ύX���I�������
	if(!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		if (stateChangeWaitTimer <= 0.0f)
		{
			//! ���~�X�e�[�g�֑J��
			TransitionDownState();
		}
	}
}

// ���~�X�e�[�g�֑J��
void EnemyChara::TransitionDownState()
{
	state = State::Down;

	//! �ʒuY��ύX����
	SetPositionYChange(-4.5f, 1.0f);

	//! �s�����x��ύX����
	SetOpacityChange(0.0f, 0.5f);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0.8f, 2.6f);

	stateChangeWaitTimer = dist(gen);
}

// ���~�X�e�[�g�֑J��
void EnemyChara::UpdateDownState(float elapsedTime)
{
	//! �ʒuY�̕ύX���I�������
	if (!positionChange.y)
	{
		stateChangeWaitTimer -= elapsedTime;

		if(stateChangeWaitTimer <= 0.0f)
		{
			modelNum = rand() % 2;

			//! �㏸�X�e�[�g�֑J��
			TransitionUpState();
		}
	}
}

// �_���[�W�X�e�[�g�֑J��
void EnemyChara::TransitionDamageState()
{
	state = State::Damage;

	positionChange.y = false;

	stateChangeWaitTimer = 1.0f;
}

// �_���[�W�X�e�[�g�X�V����
void EnemyChara::UpdateDamageState(float elapsedTime)
{
	if (!models[modelNum]->IsPlayAnimation())
	{
		//! ���~�X�e�[�g�֑J��
		TransitionDownState();
	}
}