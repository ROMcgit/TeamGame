#pragma once

#include "Graphics//Model.h"
#include "Obstacle.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// �g�Q�}��
class Togemaru : public Obstacle
{
public:
	Togemaru();
	~Togemaru() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// HP�Ȃǂ̕`��
	void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) override;

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

private:

	// �v���C���[�Ƃ̓����蔻��
	void CollisionVsPlayer();

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

private:
	// �X�e�[�g
	enum class State
	{
		Move,
	};

	// �A�j���[�V����
	enum Animation
	{
		Anim_Move,
	};

private:
	std::unique_ptr<Model> model;

	State state = State::Move;
	float territoryRange = 10.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};