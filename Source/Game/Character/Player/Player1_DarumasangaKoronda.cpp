#include <imgui.h>
#include "Player1_DarumasangaKoronda.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"

static Player1_DarumasangaKoronda* instance = nullptr;

// �C���X�^���X�擾
Player1_DarumasangaKoronda& Player1_DarumasangaKoronda::Instance()
{
	return *instance;
}

Player1_DarumasangaKoronda::Player1_DarumasangaKoronda()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	// ���f���ǂݍ���
	model = std::make_unique <Model>("Data/Model/Jammo/Jammo.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	// �q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = std::make_unique <Effect>("Data/Effect/Hit.efk");

	// �ҋ@�X�e�[�g�֑J��
	TransitionWaitState();
}

// �f�X�g���N�^
Player1_DarumasangaKoronda::~Player1_DarumasangaKoronda()
{
}

// �X�V����
void Player1_DarumasangaKoronda::Update(float elapsedTime)
{
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

	// ���͍X�V����
	UpdateVelocity(elapsedTime);

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayer1_DarumasangaKorondaVsEnemies();

	// ���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	// �e�ۂƓG�ɏՓˏ���
	CollisionProjectilesVsEnemies();

	// �����蔻��̈ʒu��ݒ�
	CollisionPosSettings();

	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();

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
	Turn3D(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	// �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
	return !(moveVec.x == 0.0f && moveVec.z == 0.0f);
}

// �U�����͏���
bool Player1_DarumasangaKoronda::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}

	return false;
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

	InputMove(elapsedTime);

	// �W�����v���͏���
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// �W�����v�X�e�[�g�֑J��
		TransitionJumpState();
	}

	InputJump();
}

// �ړ��X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionMoveState()
{
	state = State::Move;

	// ����A�j���[�V�����Đ�
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

	if(gamePad.GetAxisLX() == 0 && gamePad.GetAxisLY() == 0)
	{
		// �ҋ@�X�e�[�g�֑J��
		TransitionWaitState();
	}

	InputMove(elapsedTime);

	// �W�����v���͏���
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// �W�����v�X�e�[�g�֑J��
		TransitionJumpState();
	}

	InputJump();
}

// �W�����v�X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionJumpState()
{
	state = State::Jump;

	// �W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Jump, false);
}

// �W�����v�X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateJumpState(float elapsedTime)
{
	// �W�����v���͏���
	InputMove(elapsedTime);

	// �W�����v���͏���
	InputJump();
}

// �_���[�W�X�e�[�g�֑J��
void Player1_DarumasangaKoronda::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damage, false);
}

// �_���[�W�X�e�[�g�X�V����
void Player1_DarumasangaKoronda::UpdateDamageState(float elapsedTime)
{
	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
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

// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player1_DarumasangaKoronda::CollisionPlayer1_DarumasangaKorondaVsEnemies()
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
		//	Player1_DarumasangaKoronda::GetPosition(),
		//	Player1_DarumasangaKoronda::GetRadius(),
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
			//float diff = Player1_DarumasangaKoronda::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player1_DarumasangaKoronda::Jump(10);
			//	// ���W�����v�����邽�߂�Y�����̑��x��ݒ肷��
			//}

			//// �����o���̌�̈ʒu�ݒ�
			//enemy->SetPosition(outPosition);

			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P,E);
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
void Player1_DarumasangaKoronda::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//// �Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(collisionPos, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();

	//Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
	//debugRenderer->DrawSphere(DirectX::XMFLOAT3(
	//	leftHandBone->worldTransform._41,
	//	leftHandBone->worldTransform._42,
	//	leftHandBone->worldTransform._43),
	//	leftHandRadius,
	//	DirectX::XMFLOAT4(1, 0, 0, 1)
	//);

	// �U���Փ˗p�̍���m�[�h�̃f�o�b�O����`��
	if (attackCollisionFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(
		leftHandBone->worldTransform._41,
		leftHandBone->worldTransform._42,
		leftHandBone->worldTransform._43),
		leftHandRadius,
		DirectX::XMFLOAT4(1, 0, 0, 1)
		);
	}
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

// �W�����v���͏���
bool Player1_DarumasangaKoronda::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_A) //Z�L�[
		{
			// �W�����v�񐔐���
			if (jumpCount < jumpLimit)
			{
				// �W�����v
				Jump(jumpSpeed);
				jumpCount++;

				// �W�����v���͂���
				return true;
			}
		}
		return false;
	}
}

// �e�ۂƓG�̏Փˏ���
void Player1_DarumasangaKoronda::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(j);

			// �Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// �_���[�W��^����
				if (enemy->ApplyDamage(1, 0.5f, 0))
				{
					// �q�b�g�G�t�F�N�g�Đ�
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					// �e�۔j��
					projectile->Destroy();
				}
			}
		}
	}
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
			ImGui::InputFloat3("Position", &position.x);
			// ��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// �X�P�[��
			ImGui::InputFloat3("Scale", &scale.x);
		}
		ImGui::TreePop();
	}
}