#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Character/CollisionAttack/CollisionAttackManager.h"
#include "Game/Effect/Effect.h"

// �v���C���[
class Player5_AsibaWatari : public Character
{
public:
	Player5_AsibaWatari();
	~Player5_AsibaWatari() override;

	// �C���X�^���X�擾
	static Player5_AsibaWatari& Instance();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader, bool shadowMap = false);

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

	/*! �����蔻�菈�� */

/******************************************************************************************/

	/*! �s������ */

	// �ҋ@�X�e�[�g�֑J��
	void TransitionWaitState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateWaitState(float elapsedTime);

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

		// �_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	// �_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

	//-----------------------------------------------------------------//

		// ���S�X�e�[�g�֑J��
	void TransitionDeathState();

	// ���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTimae);

private:

	enum class State
	{
		Wait,
		Move,
		Jump,
		Damage,
		Death,
	};

	// �A�j���[�V����
	enum Animation
	{
		Anim_Wait,
		Anim_Move,
		Anim_Jump,
		Anim_Damage,
		Anim_Death,
	};

private:
	std::unique_ptr<Model> model;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 20.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;
	CollisionAttackManager collisionAttackManager;

	std::unique_ptr<Effect> hitEffect;

	State state = State::Wait;

	float playerAnimeCount = 0.0f;

	float leftHandRadius = 0.4f;

	bool attackCollisionFlag = false;
};