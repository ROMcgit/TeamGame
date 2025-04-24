#pragma once

#include "Graphics//Model.h"
#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// ��������
class EnemyOssan2 : public Enemy
{
public:
	EnemyOssan2();
	~EnemyOssan2() override;

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

	// �㏸�X�e�[�g�֑J��
	void TransitionUpState();

	// �㏸�X�e�[�g�X�V����
	void UpdateUpState(float elapsedTime);

	// ���~�X�e�[�g�֑J��
	void TransitionDownState();

	// ���~�X�e�[�g�֑J��
	void UpdateDownState(float elapsedTime);

	// �_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	// �_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

private:
	// �X�e�[�g
	enum class State
	{
		Up,
		Down,
		Damage,
	};

	State state = State::Up;

	// �A�j���[�V����
	enum Animation
	{
		Anim_Entry,
		Anim_Wait,
		Anim_Move,
		Anim_Laugh,
		Anim_Tracking,
		Anim_Tired,
		Anim_Attack,
		Anim_Death
	};

private:
	std::unique_ptr<Model> model;

	int enemyNum = 0;
};