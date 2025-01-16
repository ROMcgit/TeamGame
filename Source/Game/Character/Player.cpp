#include <imgui.h>
#include "Game/Character/Player.h"
#include "Input/Input.h"
#include "Game/Camera/Camera.h"
#include "Graphics/Graphics.h"
#include "Game/Character/Enemy/EnemyManager.h"
#include "Other/Collision.h"
#include "Game/Character/Projectile/ProjectileStraight.h"
#include "Game/Character/Projectile/ProjectileHoming.h"

static Player* instance = nullptr;

// �C���X�^���X�擾
Player& Player::Instance()
{
	return *instance;
}

Player::Player()
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
	TransitionIdleState();
}

// �f�X�g���N�^
Player::~Player()
{
}

// �X�V����
void Player::Update(float elapsedTime)
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
			state = State::Idle; // �X�e�[�g��ҋ@�ɕύX
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
		case State::Idle:
			UpdateIdleState(elapsedTime);
			break;
		case State::Move:
			UpdateMoveState(elapsedTime);
			break;
		case State::Jump:
			UpdateJumpState(elapsedTime);
			break;
		case State::Land:
			UpdateLandState(elapsedTime);
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
		case State::Revive:
			UpdateReviveState(elapsedTime);
			break;
		}
	}

	// ���͍X�V����
	UpdateVelocity(elapsedTime);

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

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
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model.get());

	// �e�ە`�揈��
	projectileManager.Render(dc, shader);
}

// HP�Ȃǂ�UI�`��
void Player::SpriteRender(ID3D11DeviceContext* dc)
{
}

// �ړ����͏���
bool Player::InputMove(float elapsedTime)
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

// �e�ۓ��͏���
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// ���i�e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_X) //C�L�[
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

		// ���ˈʒu(�v���C���[�̍�������)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + (height / 2);
		pos.z = position.z;

		// �^�[�Q�b�g(�f�t�H���g�ł̓v���C���[�̑O��)
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 10.0f; // 10.0f �͔C�ӂ̋���
		target.y = pos.y + dir.y * 10.0f;
		target.z = pos.z + dir.z * 10.0f;

		// ��ԋ߂��̓G���^�[�Q�b�g
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			// �G�Ƃ̋�������
			std::unique_ptr<Enemy>& enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5f;
			}
		}

		//����
		//ProjectileStraight * projectile = new ProjectileStraight(&projectileManager);
		//projectile->Launch(dir, pos);
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);
		
		//projectileManager.Register(projectile);
	}
}

// �U�����͏���
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}

	return false;
}

// �ҋ@�X�e�[�g�֑J��
void Player::TransitionIdleState()
{
	state = State::Idle;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Idle, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime)
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

	// �e�ۓ��͏���
	InputProjectile();

	// �U�����͏���
	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		// �U���X�e�[�g�֑J��
		TransitionAttackState();
	}
}

// �ړ��X�e�[�g�֑J��
void Player::TransitionMoveState()
{
	state = State::Move;

	// ����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Running, true);
}

// �ړ��X�e�[�g�X�V����
void Player::UpdateMoveState(float elapsedTime)
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
		TransitionIdleState();
	}

	InputMove(elapsedTime);

	// �W�����v���͏���
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// �W�����v�X�e�[�g�֑J��
		TransitionJumpState();
	}

	InputJump();

	// �e�ۓ��͏���
	InputProjectile();

	// �U�����͏���
	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		// �U���X�e�[�g�֑J��
		TransitionAttackState();
	}
}

// �W�����v�X�e�[�g�֑J��
void Player::TransitionJumpState()
{
	state = State::Jump;

	// �W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Jump, false);
}

// �W�����v�X�e�[�g�X�V����
void Player::UpdateJumpState(float elapsedTime)
{
	if (jumpCount == 2 && jumpFlipAnimation == false)
	{
		model->PlayAnimation(Anim_Jump_Flip, false);
		jumpFlipAnimation = true;
	}

	// �W�����v���͏���
	InputMove(elapsedTime);

	// �W�����v���͏���
	InputJump();

	// �e�ۓ��͏���
	InputProjectile();
}

// ���n�X�e�[�g�֑J��
void Player::TransitionLandState()
{
	state = State::Land;

	jumpFlipAnimation = false;

	// ���n�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Landing, false);
}

// ���n�X�e�[�g�X�V����
void Player::UpdateLandState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		// �ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}
}

// �U���X�e�[�g�֑J��
void Player::TransitionAttackState()
{
	state = State::Attack;

	// ���n�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack, false, 0.2f);

	playerAnimeCount = 0.0f;
}

// �U���X�e�[�g�X�V����
void Player::UpdateAttackState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		// �ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}

	playerAnimeCount += elapsedTime;

	// �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓˏ������������
	float animationTime = static_cast<float>(playerAnimeCount) / 45.0f;
	if (animationTime >= 0.001f && animationTime <= 0.01f)
		attackCollisionFlag = true;
	else 
		attackCollisionFlag = false;

	// �Փ˔���t���O�������Ă���ꍇ�A����m�[�h�ƃG�l�~�[�̏Փˏ������s��
	if (attackCollisionFlag)
	{
		// ����m�[�h�ƃG�l�~�[�̏Փˏ���
		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
}

// �m�[�h�ƓG�̏Փˏ���
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
	// �m�[�h�擾
	Model::Node* node = model->FindNode(nodeName);

	// �m�[�h�����݂��Ȃ��ꍇ�͏������I��
	if (!node) return;

	// �m�[�h�ʒu�擾
	DirectX::XMMATRIX nodeTransform = DirectX::XMLoadFloat4x4(&node->worldTransform);
	DirectX::XMVECTOR nodePosVector = nodeTransform.r[3];
	DirectX::XMFLOAT3 nodePosition;
	DirectX::XMStoreFloat3(&nodePosition, nodePosVector);

	// �G�}�l�[�W���[�̃C���X�^���X���擾
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �w��̃m�[�h�ƑS�Ă̓G�𑍓�����ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		std::unique_ptr<Enemy>& enemy = enemyManager.GetEnemy(i);

		// �Փˏ���
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsSphere(
			nodePosition,
			nodeRadius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			outPosition
		))
		{
			// �����o���̌�̈ʒu�ݒ�
			enemy->SetPosition(outPosition);

			enemy->ApplyDamage(1, 4.5f, 0);
		}
	}
}

// �_���[�W�X�e�[�g�֑J��
void Player::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_GetHit1, false);
}

// �_���[�W�X�e�[�g�X�V����
void Player::UpdateDamageState(float elapsedTime)
{
	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

// ���S�X�e�[�g�֑J��
void Player::TransitionDeathState()
{
	state = State::Death;

	// ���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void Player::UpdateDeathState(float elapsedTimae)
{
	if (!model->IsPlayAnimation())
	{
		// �{�^�����������畜���X�e�[�g�֑J��
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			TransitionReviveState();
		}
	}
}

// �����X�e�[�g�֑J��
void Player::TransitionReviveState()
{
	state = State::Revive;

	// �̗͉�
	hp = maxHp;

	// �����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Reving, false);
}

// �����X�e�[�g�X�V����
void Player::UpdateReviveState(float elapsedTime)
{
	// �����A�j���[�V�����I����ɑҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player::CollisionPlayerVsEnemies()
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
		//	Player::GetPosition(),
		//	Player::GetRadius(),
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
			//float diff = Player::GetPosition().y - ( enemy->GetPosition().y + enemy->GetHeight());
			//if (diff < -0.2f)
			//{
			//	Player::Jump(10);
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
void Player::DrawDebugPrimitive()
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
void Player::OnLanding()
{
	// �������̑��͂����ȏ�Ȃ璅�n�X�e�[�g��
	//if (velocity.y <= -5)
	//{
	//	TransitionLandState();
	//}

	// �_���[�W�A���S�X�e�[�g���͒��n�������ɃX�e�[�g�J�ڂ��Ȃ��悤�ɂ���
	if (velocity.y <= -5 && state != State::Damage && state != State::Death)
	{
		TransitionLandState();
	}

	jumpCount = 0;
}

// �_���[�W���󂯂����ɌĂ΂��
void Player::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}

// �W�����v���͏���
bool Player::InputJump()
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
void Player::CollisionProjectilesVsEnemies()
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
void Player::DrawDebugGUI()
{
	if (ImGui::TreeNode("Player"))
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