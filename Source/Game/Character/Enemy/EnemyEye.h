#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// ��
class EnemyEye : public Enemy
{
public:
	EnemyEye();
	~EnemyEye() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HP�Ȃǂ̕`��
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) override;

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

	// �f�o�b�OGUI
	void DrawDebugGUI() override;

protected:
	//�_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;

	// ���S�������ɌĂ΂��
	void OnDead() override;

private:

	// �v���C���[��T��
	bool SearchPlayer();

	// �ړ��ʒu�Ɉړ�
	void MoveTarget(float elapsedTime, float speedRate);

	// �ҋ@�X�e�[�g�֑J��
	void TransitionWaitState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateWaitState(float elapsedTime);

	// ���S�X�e�[�g�֑J��
	void TransitionDeathState();

	// ���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTime);

private:
	// �X�e�[�g
	enum class State
	{
		Wait,
	};

private:
	std::unique_ptr<Model> model;

	bool deathState = false;

	bool setMoveTarget = false;

	bool playAnimation = false;
	float dist = 0;
	DirectX::XMFLOAT3 moveTarget = { 0, 0, 0 }; // �ʒu�ړ��̃^�[�Q�b�g(�v���C���[�Ƃ͕�)

	State state = State::Wait;
	float territoryRange = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 12.0f;
	float attackRange = 1.5f;

	bool rotationR = false;
};