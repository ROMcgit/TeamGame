#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// �S
class EnemyOni : public Enemy
{
public:
	EnemyOni();
	~EnemyOni() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HP�Ȃǂ̕`��
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) override;

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

protected:
	//�_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;

	// ���S�������ɌĂ΂��
	void OnDead() override;

private:

	// �m�[�h�ƃv���C���[�̏Փˏ���
	void CollisionNodeVsPlayer(const char* nodeName, float nodeRadius);

	// �ҋ@�X�e�[�g�֑J��
	void TransitionWaitState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateWaitState(float elapsedTime);

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

	// �Њd�X�e�[�g�֑J��
	void TransitionThreatState();

	// �Њd�X�e�[�g�X�V����
	void UpdateThreatState(float elapsedTime);

	// �ǐՃX�e�[�g�֑J��
	void TransitionTrackingState();

	// �ǐՃX�e�[�g�X�V����
	void UpdateTrackingState(float elapsedTime);

	// �U���X�e�[�g�֑J��
	void TransitionAttackState();

	// �U���X�e�[�g�X�V����
	void UpdateAttackState(float elapsedTime);

	// �_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	// �_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

	// ���S�X�e�[�g�֑J��
	void TransitionDeathState();

	// ���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTime);

private:
	// �X�e�[�g
	enum class State
	{
		Wait,
		Move,
		Threat,
		Tracking,
		Attack,
		Damage,
		Death
	};

	// �A�j���[�V����
	enum Animation
	{
		Anim_Wait,
		Anim_Move,
		Anim_Attack,
		Anim_Damage,
		Anim_Death
	};

private:
	std::unique_ptr<Model> model;

	State state = State::Wait;
	float territoryRange = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};