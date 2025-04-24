#pragma once

#include "Graphics//Model.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"
#include "Item.h"

// ���D_�v���X
class Balloon_Minus : public Item
{
public:
	Balloon_Minus();
	~Balloon_Minus() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

	// �f�o�b�OGUI
	void DrawDebugGUI() override;

	// �ʒuY�̃��Z�b�g�̐���
	void SetPositionResetY(float positionResetY) { this->positionResetY = positionResetY; }

protected:
	//�_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;

	// ���S�������ɌĂ΂��
	void OnDead() override;

private:

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

	// ���S�X�e�[�g�֑J��
	void TransitionBreakState();

	// ���S�X�e�[�g�X�V����
	void UpdateBreakState(float elapsedTime);

private:
	// �X�e�[�g
	enum class State
	{
		Move,
		Break,
	};

	State state = State::Move;

private:
	std::unique_ptr<Model> model;

	float positionResetY = 0.0f;
};