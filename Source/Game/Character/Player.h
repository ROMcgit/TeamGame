#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"

// �v���C���[
class Player : public Character
{
public:
	Player();
	~Player() override;

	// �C���X�^���X�擾
	static Player& Instance();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// HP�Ȃǂ�UI�`��
	void SpriteRender(ID3D11DeviceContext* dc);

	// �f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

protected:
	// ���n�������ɌĂ΂��
	void OnLanding() override;

	// �_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;

	// ���S�������ɌĂ΂��
	void OnDead() override;

private:

	// �ړ����͏���
	bool InputMove(float elapsedTime);

	// �W�����v���͏���
	bool InputJump();

	// �e�ۓ��͏���
	void InputProjectile();

	// �U�����͏���
	bool InputAttack();

	/*! �����蔻�菈�� */

	// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	void CollisionPlayerVsEnemies();

	// �m�[�h�ƃG�l�~�[�̏Փˏ���
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	// �e�ۂƓG�̏Փˏ���
	void CollisionProjectilesVsEnemies();

/******************************************************************************************/

	/*! �s������ */

	// �ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

//-----------------------------------------------------------------//

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

//-----------------------------------------------------------------//

	// �W�����v�X�e�[�g�֑J��
	void TransitionJumpState();

	// �W�����v�X�e�[�g�X�V����
	void UpdateJumpState(float elapsedTime);

//-----------------------------------------------------------------//

	// ���n�X�e�[�g�֑J��
	void TransitionLandState();

	// ���n�X�e�[�g�X�V����
	void UpdateLandState(float elapsedTime);

//-----------------------------------------------------------------//

	// �U���X�e�[�g�֑J��
	void TransitionAttackState();

	// �U���X�e�[�g�X�V����
	void UpdateAttackState(float elapsedTime);

//-----------------------------------------------------------------//

	// �_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	// �_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

//-----------------------------------------------------------------//

	// ���S�X�e�[�g�֑J��
	void TransitionDeathState();

	// ���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTimae);

//-----------------------------------------------------------------//

	// �����X�e�[�g�֑J��
	void TransitionReviveState();

	// �����X�e�[�g�X�V����
	void UpdateReviveState(float elapsedTime);

private:

	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Attack,
		Damage,
		Death,
		Revive
	};

	// �A�j���[�V����
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_Jump,
		Anim_Jump_Flip,
		Anim_Landing,
		Anim_Reving,
		Anim_Running,
		Anim_Walking
	};

private:
	std::unique_ptr<Model> model;
	
	float moveSpeed = 5.5f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	std::unique_ptr<Effect> hitEffect;

	State state = State::Idle;

	float playerAnimeCount = 0.0f;

	float leftHandRadius = 0.4f;

	bool attackCollisionFlag = false;
};