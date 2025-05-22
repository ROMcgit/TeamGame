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
#include "Game/Scene/G3_SoratobuHusenWari.h"
#include "Audio/SoundManager.h"

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
	scale.x = scale.y = scale.z = 0.022f;

	debugPrimitiveColor = { 0, 0, 1 };

	radius = 1.1f;
	height = 3.1f;

	collisionOffset.z = -0.17f;

	opacity = 0.8f;

	// �d��
	gravity = gravityReset = 0.25f;

	//! �v���C���[UI
	//uiSprite[0] = std::make_unique <Sprite>("Data/Sprite/5.UI/CuppyUI.png");
	for (int i = 0; i < 3; i++)
	{
		uiSprite[i + 1] = std::make_unique <Sprite>(); // HP�Q�[�W
		//[1] �D�F
		//[2] �ԐF
		//[3] �ΐF
	}

	hp = maxHp = hpDamage = 200;

	hpImagePos = { 8.0f, 15 };
	hpImageShakePosY = hpImagePos.y;

	hpSpritePos = { 15, 15 };
	hpSpriteShakePosY = hpSpritePos.y;

	playerHpSpriteWidth = 200;

	SoundManager& sound = SoundManager::Instance();
	sound.LoadSound("���n", "Data/Audio/Sound/Attack.wav");
	sound.LoadSound("��", "Data/Audio/Sound/Thunder.wav");

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

	// �L�����N�^�[��ԍX�V����
	UpdateGameObjectBaseState(elapsedTime);

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	// ���f���X�V����
	model->UpdateTransform(transform);

	if (G3_SoratobuHusenWari::gameTimer < 2.0f)
	{
		gravity = 0.0f;

		return;
	}
	else if(!gameStart)
	{
		gravity = gravityReset;

		gameStart = true;
	}

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

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �ՓˍU���X�V����
	collisionAttackManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayer3_SoratobuHusenWariVsEnemies();

	// �ʒu����
	PositionControll(elapsedTime);

	UpdateHpPinch(elapsedTime);
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
	float textureWidth = static_cast<float>(uiSprite[1]->GetTextureWidth());
	float textureHeight = static_cast<float>(uiSprite[1]->GetTextureHeight());

	if (!hideSprites)
	{
		// �Q�[�W�̗�(�D�F)
		uiSprite[1]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			playerHpSpriteWidth, 40,
			0, 0, textureWidth, textureHeight,
			0,
			0.3f, 0.3f, 0.3f, 1);

		float hpWidth = (static_cast<float>(hp) / maxHp) * playerHpSpriteWidth; // HP�̉��̒���
		float hpDamageWidth = (static_cast<float>(hpDamage) / maxHp) * playerHpSpriteWidth; // HP�_���[�W�̉��̒���

		// �_���[�W(�ԐF)
		uiSprite[2]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			hpDamageWidth, 40,
			0, 0, textureWidth, textureHeight,
			0,
			1, 0, 0, 1);

		// HP�Q�[�W
		uiSprite[3]->Render(dc,
			hpSpritePos.x, hpSpritePos.y,
			hpWidth, 40,
			0, 0, textureWidth, textureHeight,
			0,
			hpSpriteColorP.x,
			hpSpriteColorP.y,
			hpSpriteColorP.z,
			1);

		//textureWidth = static_cast<float>(uiSprite[0]->GetTextureWidth());
		//textureHeight = static_cast<float>(uiSprite[0]->GetTextureHeight());

		// �v���C���[UI�摜
		/*uiSprite[0]->Render(dc,
			hpImagePos.x, hpImagePos.y,
			241, 84,
			0, 0, textureWidth, textureHeight,
			0,
			hpImageColor.x,
			hpImageColor.y,
			hpImageColor.z,
			1);*/
	}
}

// �ʒu����
void Player3_SoratobuHusenWari::PositionControll(float elapsedTime)
{
	//! �ʒuX�𐧌�
	if (position.x < -10.2f || position.x > 10.2f)
	{
		velocity.x = 0;
		position.x = std::clamp(position.x, -10.2f, 10.2f);
	}

	if (position.y > 25.0f)
	{
		position.y = 25.0f;

		velocity.y -= 5 * elapsedTime;
	}
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

// HP�s���`�X�V����
void Player3_SoratobuHusenWari::UpdateHpPinch(float elapsedTime)
{
	//! HP���s���`����Ȃ��Ȃ�A�������~�߂�
	if (hp > maxHp * 0.28f || hpShake)
	{
		hpPinchTimer = 0.0f; // �^�C�}�[��0�ɂ���

		//! �}�e���A���̐F��1�ɂ���
		materialColor.x = materialColor.y = materialColor.z = 1.0f;

		//! �G�~�b�V�u�̋�����ύX
		if (emissiveStrength > 0.0f && !emissivePhaseChange)
			SetEmissiveStrengthChange(DirectX::XMFLOAT3(0, 0, 0), 0.0f, 0.1f);

		return;
	}

	//! �^�C�}�[�𑝂₷
	hpPinchTimer += elapsedTime;

	//! �^�C�}�[�Ɋ�Â��ĐF��؂�ւ�
	const float blinkInterval = 0.15f; //! �_�ŊԊu
	if (fmod(hpPinchTimer, blinkInterval * 2) < blinkInterval) {
		hpSpriteColorP = { 1.0f, 0.412f, 0.706f }; // �s���N�F
	}
	else
	{
		hpSpriteColorP = { 1.0f, 1.0f, 1.0f }; // ���F
	}

	//! �G�~�b�V�u�̋�����ύX����
	if (emissiveStrength == 0.0f)
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 1.0f, 0.2f);
	//! 0����Ȃ��Ȃ�
	else
		SetEmissiveStrengthChange(DirectX::XMFLOAT3(1, 0, 0), 0.0f, 0.2f);
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

	float move = 0;

	if (G3_SoratobuHusenWari::gameTimer < 30.0f)
		move = 5.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 100.0f)
		move = 8.0f;
	else if (G3_SoratobuHusenWari::gameTimer < 150.0f)
		move = 12.0f;
	else
		move = 17.0f;


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
	GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y | GamePad::BTN_SPACE;
	if (gamePad.GetButtonHeld() & button)
	{
		velocity.y += 100 * elapsedTime;
	}

	velocity.y = std::clamp(velocity.y, -5.0f, 5.0f);


	if (isDamage || isGround)
	{
		int damage = isDamage ? 40 : 20;
		//! �_���[�W��^����
		ApplyDamage(damage, 0.1f);

		std::string soundPath = isDamage ? "��" : "���n";
		float volume = isDamage ? 1.5f : 1.0f;
		SoundManager::Instance().PlaySound(soundPath.c_str(), volume);

		TransitionDamageState();
	}
}

// �_���[�W�X�e�[�g�֑J��
void Player3_SoratobuHusenWari::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Damage, false);

	hpShake = true;

	if(!isGround)
		velocity.y = 0.0f;
	else
		velocity.y = 10.0f;

	gravity = 0.13f;

	//! �J���[�t�B���^�[��ύX����
	SetColorFilterChange(DirectX::XMFLOAT3(Camera::colorFilterReset.x + 1.7f, Camera::colorFilterReset.y, Camera::colorFilterReset.z), 0.5f);

	//! �J�����V�F�C�N
	SetCameraShake(0.2f, DirectX::XMINT3(10, 60, 0));

	stateChangeWaitTimer = 1.2f;
}

// �_���[�W�X�e�[�g�X�V����
void Player3_SoratobuHusenWari::UpdateDamageState(float elapsedTime)
{
	stateChangeWaitTimer -= elapsedTime;

	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (stateChangeWaitTimer <= 0.0f)
	{
		gravity = gravityReset;

		//! �J���[�t�B���^�[��߂�
		SetColorFilterResetChange(0.3f);

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

		ImGui::DragInt("HP", &hp);
		ImGui::DragFloat2("HPPos", &hpSpritePos.x, 0.2f);
		ImGui::DragFloat("HPWidth", &playerHpSpriteWidth, 0.2f);

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