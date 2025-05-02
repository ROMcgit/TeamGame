#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Game/Character/Character.h"
#include "Game/Character/Projectile/ProjectileManager.h"
#include "Game/Effect/Effect.h"
#include "Game/Character/CollisionAttack/CollisionAttackManager.h"

// �v���C���[
class Player3_SoratobuHusenWari : public Character
{
public:
	Player3_SoratobuHusenWari();
	~Player3_SoratobuHusenWari() override;

	// �C���X�^���X�擾
	static Player3_SoratobuHusenWari& Instance();

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
	// �ʒu����
	void PositionControll(float elapsedTime);

	// �ړ����͏���
	bool InputMove(float elapsedTime);

	/*! �����蔻�菈�� */

	// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	void CollisionPlayer3_SoratobuHusenWariVsEnemies();

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

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
		Move,
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

public:
	bool isDamage = false;

private:
	std::unique_ptr<Model> model;

	std::unique_ptr<Sprite> uiSprite[4];

	// �ՓˍU��
	CollisionAttackManager collisionAttackManager;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 13.0f;

	int jumpCount = 0;
	bool jumpFlipAnimation = false;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	std::unique_ptr<Effect> hitEffect;

	State state = State::Move;

	float playerAnimeCount = 0.0f;

	float leftHandRadius = 0.4f;

	bool attackCollisionFlag = false;
};