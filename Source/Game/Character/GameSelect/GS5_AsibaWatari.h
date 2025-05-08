#pragma once

#include "Graphics//Model.h"
#include "GameSelect.h"
#include "Graphics/Graphics.h"
#include "Other/Mathf.h"

// ����n��
class GS5_AsibaWatari : public GameSelect
{
public:
	GS5_AsibaWatari();
	~GS5_AsibaWatari() override;

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

	// �꒣��ݒ�
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

private:
	// �^�[�Q�b�g�ʒu�������_���ݒ�
	void SetRandomTargetPosition();

	// �ڕW�n�_�ֈړ�
	void MoveToTarget(float elapsedTime, float speedRate);

	// �ҋ@�X�e�[�g�֑J��
	void TransitionWaitState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateWaitState(float elapsedTime);

	// �v���C���[�Ƃ̐ڐG����
	void CollisionVsPlayer();

private:
	// �X�e�[�g
	enum class State
	{
		Wait,
	};

	// �A�j���[�V����
	enum Animation
	{
		Anim_Wait,
	};

private:
	std::unique_ptr<Model> model;

	State state = State::Wait;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};