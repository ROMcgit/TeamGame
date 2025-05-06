#include <imgui.h>
#include "Player2_Sundome.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Camera/CameraController.h"
#include "Game/Scene/G2_Sundome.h"
#include "Game/Scene/SceneManager.h"
#include "Game/Scene/SceneLoading.h"
#include "Game/Scene/G2_Sundome_Result.h"

static Player2_Sundome* instance = nullptr;

// �C���X�^���X�擾
Player2_Sundome& Player2_Sundome::Instance()
{
	return *instance;
}

Player2_Sundome::Player2_Sundome()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	// ���f���ǂݍ���
	models[0] = std::make_unique <Model>("Data/Model/2.Sundome/Suika/Suika.mdl");
	models[1] = std::make_unique <Model>("Data/Model/2.Sundome/TogeBall/TogeBall.mdl");
	models[2] = std::make_unique <Model>("Data/Model/2.Sundome/Person/Person.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.03f;
	scaleReset = scale;

	debugPrimitiveColor = { 1, 0, 0 };

	collisionType = CollisionType::Sphere;

	radius = 3.03f;

	gravity = 0.5f;

	// �t�F�[�h
	fade = std::make_unique<Fade>();

	// �����x�̐��l
	velocityText = std::make_unique<Text>();
	velocityTextSyosuten = std::make_unique<Text>();

	// �����x�̉摜
	velocitySprite = std::make_unique<Sprite>();

	// �ҋ@�X�e�[�g�֑J��
	TransitionMovieWaitState();
}

// �f�X�g���N�^
Player2_Sundome::~Player2_Sundome()
{
}

// �X�V����
void Player2_Sundome::Update(float elapsedTime)
{
	if (CameraController::debugCamera) return;

	fade->Update(elapsedTime);

	// �X�e�[�g���̏���
	switch (state)
	{
	case State::MovieWait:
		UpdateMovieWaitState(elapsedTime);
		break;
	case State::Wait:
		UpdateWaitState(elapsedTime);
		break;
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	case State::Return:
		UpdateReturnState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	}

	// �L�����N�^�[��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayer2_SundomeVsEnemies();

	// ���f���X�V����
	for(int i = 0; i < 3; i++)
		models[i]->UpdateTransform(transform);
}

// �`�揈��
void Player2_Sundome::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, models[round - 1].get());

	// �e�ە`�揈��
	projectileManager.Render(dc, shader);
}

// HP�Ȃǂ�UI�`��
void Player2_Sundome::SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics)
{
	float textureWidth = static_cast<float>(velocitySprite->GetTextureWidth());
	float textureHeight = static_cast<float>(velocitySprite->GetTextureHeight());

	float velocityHeight = (setVelocityX / velocityLimit.max) * spriteScale.y;

	if(!G2_Sundome::movieScene)
	{
		//! �����x�̕`��
		if(state == State::Wait)
		{
			velocitySprite->Render(dc,
				spritePos.x, spritePos.y,
				spriteScale.x, velocityHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 0, 0, 1);
		}

		float velocityX = abs((int)velocity.x);
		double velocityXSyosuten = abs(velocity.x) - velocityX;
		velocityXSyosuten = (int)(velocityXSyosuten * 10);

		//! �����x�̐��l�̕`��
		velocityText->RenderOku(dc, false, velocityX, false,
			velocityTextPos.x, velocityTextPos.y);

		//! �����x�̐��l(�����_)�̕`��
		velocityTextSyosuten->RenderOku(dc, false, velocityXSyosuten, false,
			velocityTextPos.x + 50.0f, velocityTextPos.y);
	}

	//! �t�F�[�h
	fade->Render(dc, graphics);
}

// �ړ����͏���
bool Player2_Sundome::InputMove(float elapsedTime)
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

// ���[�r�[�ҋ@�X�e�[�g�֑J��
void Player2_Sundome::TransitionMovieWaitState()
{
	state = State::MovieWait;

	position = { 215.0f, 70.0f, -2.0f };

	scale = scaleReset;

	angle = { 0, 0, 0 };

	switch (round)
	{
	case 1: break;
	case 2:
	{
		angle.x = DirectX::XMConvertToRadians(-90);
		angle.y = DirectX::XMConvertToRadians(90);
	}
		break;
	case 3:
	{
		angle.x = DirectX::XMConvertToRadians(-90);
		angle.y = DirectX::XMConvertToRadians(90);

		scale.x = scale.y = scale.z = 0.05f;
	}
		break;
	default:
		break;
	}
}

// ���[�r�[�ҋ@�X�e�[�g�X�V����
void Player2_Sundome::UpdateMovieWaitState(float elapsedTime)
{
	if (!G2_Sundome::movieScene)
		TransitionWaitState();
}

// �ҋ@�X�e�[�g�֑J��
void Player2_Sundome::TransitionWaitState()
{
	state = State::Wait;
}

// �ҋ@�X�e�[�g�X�V����
void Player2_Sundome::UpdateWaitState(float elapsedTime)
{
	setVelocityX += (30 * elapsedTime) * (velocityDown ? -1 : 1);

	if (setVelocityX < velocityLimit.min || setVelocityX > velocityLimit.max)
	{
		setVelocityX = std::clamp(setVelocityX, velocityLimit.min, velocityLimit.max);
	
		//! �����x�𔽓]
		velocityDown = !velocityDown;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	GamePadButton button =
	GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	if (gamePad.GetButtonDown() & button)
	{
		//! �ړ��X�e�[�g�ɑJ��
		TransitionMoveState();
	}
}

// �ړ��X�e�[�g�֑J��
void Player2_Sundome::TransitionMoveState()
{
	state = State::Move;

	//! �u���[�L
	switch (round)
	{
	case 1: brake = 8.0f; break;
	case 2: brake = 25.0f; break;
	case 3: brake = 30.0f; break;
	default:
		break;
	}

	stateChangeWaitTimer = 1.0f;
}

// �ړ��X�e�[�g�X�V����
void Player2_Sundome::UpdateMoveState(float elapsedTime)
{
	if (round == 3)
		brake = rand() % 30 + 13.0f;

	velocity.x = setVelocityX * -1;

	float rotationSpeed = setVelocityX / velocityLimit.max * 600;

	switch (round)
	{
	case 1:
	{
		angle.z += DirectX::XMConvertToRadians(rotationSpeed) * elapsedTime;
	}
		break;
	case 2:
	{
		angle.x -= DirectX::XMConvertToRadians(rotationSpeed) * elapsedTime;
	}
		break;
	case 3:
	{
		angle.x -= DirectX::XMConvertToRadians(rotationSpeed) * elapsedTime;
	}
		break;
	default:
		break;
	}

	GamePad& gamePad = Input::Instance().GetGamePad();

	float velocityX = abs((int)velocity.x);
	double velocityXSyosuten = abs(velocity.x) - velocityX;
	velocityXSyosuten = (int)(velocityXSyosuten * 10);

	GamePadButton button =
		GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y;

	if ((gamePad.GetButtonHeld() & button && position.x < 205.0f
		&& ((setVelocityX > 0.0f && velocityXSyosuten >= 0.0f) || (setVelocityX <= 0.0f && velocityXSyosuten > 0.0f)) && isGround)
		|| setVelocityX <= 13.0f && ((setVelocityX > 0.0f && velocityXSyosuten >= 0.0f) || (setVelocityX <= 0.0f && velocityXSyosuten > 0.0f)))
		setVelocityX -= brake * elapsedTime;
	else if ((int)setVelocityX <= 0.0f && velocityXSyosuten <= 0.0f)
		setVelocityX = 0.0f;

	//! ���E���h��2�Œ��n���Ă���Ȃ�
	if (round == 2 && isGround && boundTimer > 0.7f && setVelocityX > 0.0f)
	{
		boundTimer = 0.0f;
		velocity.y = 5.0f;
	}
	else
		boundTimer += elapsedTime;

	//! �ʒu��30��菬�����Ȃ�
	if (position.y < 30.0f)
	{
		//! ���S�X�e�[�g�֑J��
		TransitionDeathState();
	}
	else if (setVelocityX == 0)
	{
		stateChangeWaitTimer -= elapsedTime;

		if(stateChangeWaitTimer <= 0.0f)
		{
			TransitionReturnState();
		}
	}
}

// �߂�X�e�[�g�֑J��
void Player2_Sundome::TransitionReturnState()
{
	state = State::Return;

	setVelocityX = 0.0f;

	//! �t�F�[�h��ݒ�
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		0.0f, 1.0f,
		1.0f, 0.2f);

	stateChangeWaitTimer = 0.5f;
}

// �߂�X�e�[�g�X�V����
void Player2_Sundome::UpdateReturnState(float elapsedTime)
{
	if (!fade->GetFade())
	{
		stateChangeWaitTimer -= elapsedTime;

		if (stateChangeWaitTimer <= 0.0f)
		{
			G2_Sundome::movieScene = true;

			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				1.0f, 0.0f,
				1.0f, 0.6f);

			if (round < 3)
				round++;
			else
			{
				std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G2_Sundome_Result>());

				// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
				SceneManager::Instance().ChangeScene(std::move(loadingScene));
			}

			//! ���[�r�[�ҋ@�X�e�[�g�֑J��
			TransitionMovieWaitState();
		}
	}
}

// �_���[�W�X�e�[�g�֑J��
void Player2_Sundome::TransitionDamageState()
{
	state = State::Damage;

	setVelocityX = 0.0f;
}

// �_���[�W�X�e�[�g�X�V����
void Player2_Sundome::UpdateDamageState(float elapsedTime)
{
	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionWaitState();
	}
}

// ���S�X�e�[�g�֑J��
void Player2_Sundome::TransitionDeathState()
{
	state = State::Death;

	setVelocityX = 0.0f;

	DirectX::XMFLOAT3 colorF = Camera::postEffect.colorFilter;
	colorF.x += 2.0f;
	
	// �J���[�t�B���^�[��ύX����
	SetColorFilterChange(DirectX::XMFLOAT3(colorF), 0.5f);

	//! �t�F�[�h��ݒ�
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		0.0f, 1.0f,
		1.0f, 0.5f);

	stateChangeWaitTimer = 1.0f;
}

// ���S�X�e�[�g�X�V����
void Player2_Sundome::UpdateDeathState(float elapsedTimae)
{
	float scale = 0.0f;
	SetScaleAllChange(DirectX::XMFLOAT3(scale, scale, scale), 1.5f);

	if (!scaleChange.all && !fade->GetFade())
	{
		stateChangeWaitTimer -= elapsedTimae;

		if (stateChangeWaitTimer <= 0.0f)
		{
			G2_Sundome::movieScene = true;

			fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
				1.0f, 0.0f,
				1.0f, 0.6f);

			DirectX::XMFLOAT3 colorF = Camera::colorFilterReset;
			// �J���[�t�B���^�[��ύX����
			SetColorFilterChange(DirectX::XMFLOAT3(colorF), 0.5f);

			if (round < 3)
				round++;
			else
			{
				std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<G2_Sundome_Result>());

				// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
				SceneManager::Instance().ChangeScene(std::move(loadingScene));
			}

			//! ���[�r�[�ҋ@�X�e�[�g�֑J��
			TransitionMovieWaitState();
		}
	}
}

// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player2_Sundome::CollisionPlayer2_SundomeVsEnemies()
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
		//	Player2_Sundome::GetPosition(),
		//	Player2_Sundome::GetRadius(),
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
			//float diff = Player2_Sundome::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player2_Sundome::Jump(10);
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
void Player2_Sundome::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

#ifndef _DEBUG
	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawSphere(collisionPos, radius, { debugPrimitiveColor.x, debugPrimitiveColor.y, debugPrimitiveColor.z, 1 });

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();

#endif // !_DEBUG
}

// ���n�������ɌĂ΂��
void Player2_Sundome::OnLanding()
{
}

// �_���[�W���󂯂����ɌĂ΂��
void Player2_Sundome::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player2_Sundome::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �����蔻��`��
void Player2_Sundome::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player2_Sundome"))
	{
		ImGui::DragFloat2("SpritePos", &spritePos.x, 0.1f);
		ImGui::DragFloat2("SpriteScale", &spriteScale.x);

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