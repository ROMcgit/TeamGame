#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Shader.h"
#include "Game/Effect/Effect.h"
#include "../GameObjectBase.h"
#include <stdexcept>
#include <algorithm>

// �O���錾
class CollisionAttackManager;

// �ՓˍU��
class CollisionAttack : public GameObjectBase
{
public:
	CollisionAttack(CollisionAttackManager* manager);
	virtual ~CollisionAttack() {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2D�X�v���C�g�`��
	virtual void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) {}

	// �f�o�b�O�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	// �f�o�b�OGUI
	virtual void DrawDebugGUI() {}

	// �G�t�F�N�g�̃n���h�����擾
	virtual Effekseer::Handle GetHandle() { return -1; }

	// �j��
	void Destroy();

	//! �v���C���[3�Ƃ̐ڐG����
	bool CollisionVsPlayer3();

/*****************************************************************************************************************************/
	/*! �Z�b�^�[ */
#if 1

	/*! �_���[�W�֘A */
	void SetOpponentState(int damage, float invincibleTime = 1.5f, float defensesUpTime = 1.0f, const DirectX::XMFLOAT2& knockback = { 10.0f, 6.0f })
	{
		this->opponentDamage         = damage;
		this->opponentInvincibleTime = invincibleTime;
		this->opponentDefensesUpTime = defensesUpTime;
		this->opponentKnockback      = knockback;
	}

#endif

/*****************************************************************************************************************************/
	/*! �Q�b�^�[ */
#if 1

	/*! �_���[�W�֘A */

	// �v���C���[�Ȃǂɗ^����_���[�W���擾
	int GetOpponentDamage() { return opponentDamage; }

	// �v���C���[�Ȃǂɗ^���閳�G���Ԃ��擾
	float GetOpponentInvincibleTime() { return opponentInvincibleTime; }

	// �v���C���[�Ȃǂɗ^����h��̓A�b�v���Ԃ��擾
	float GetOpponentDefensesUpTime() { return opponentDefensesUpTime; }

	// �v���C���[�Ȃǂɗ^����m�b�N�o�b�N���擾
	const DirectX::XMFLOAT2& GetOpponentKnockback() const { return opponentKnockback; }

#endif

public:
	static bool stopCollisionAttack; // �ՓˍU�����~�߂邩(�s��X�V�����ȊO)

protected:
	CollisionAttackManager* manager = nullptr;

	float lifeTimer = 1.0f; // ��������

	/*! �_���[�W�֘A */
#if 1

	int   opponentDamage                = 12;              // �v���C���[�Ȃǂɗ^����_���[�W
	float opponentInvincibleTime        = 1.5f;            // �v���C���[�Ȃǂɗ^���閳�G����
	float opponentDefensesUpTime        = 1.0f;            // �v���C���[�Ȃǂɗ^����h��̓A�b�v����
	DirectX::XMFLOAT2 opponentKnockback = { 10.0f, 6.0f }; // �v���C���[�Ȃǂɗ^����m�b�N�o�b�N

#endif
};