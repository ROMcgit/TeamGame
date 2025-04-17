#pragma once

#include "GameObjectBase.h"

// �L�����N�^�[
class Character : public GameObjectBase, public std::enable_shared_from_this<Character>
{
public:
	Character() {}
	virtual ~Character() {}

	// �_���[�W��^����
	bool ApplyDamage(int damage, float invincibleTime, float defensesUpTime = 0.0f);

	/**************************************************************************************/

		/*! �Q�b�^�[ */
#if 1

//---------------------------------------------------------------------------//

	/*! �G�֘A */
#if 1

	// ���f���̎擾
	virtual Model* GetModel() { return nullptr; }

	// �w�肵���C���f�b�N�X�̃��f�����擾
	virtual Model* GetModels(int modelNum) { return nullptr; }

	// �t�F�[�h���擾
	virtual Fade* GetFade() { return nullptr; }

	// �^�[�Q�b�g�̈ʒu���擾
	const DirectX::XMFLOAT3& GetTargetPosition() const { return targetPosition; }

#endif

	//---------------------------------------------------------------------------//

		/*! �}�l�[�W���[ */
#if 1


#endif

#endif // �Q�b�^�[

protected:

	//!\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
		/*! �X�V�����Ȃ� */

	/**************************************************************************************/

		/*! �ړ��֘A */
#if 1

	// �X�e�B�b�N���͒l����ړ��x�N�g�����K��(3D)
	DirectX::XMFLOAT3 GetMoveVec() const;

	// �X�e�B�b�N���͒l����ړ��x�N�g�����K��(2D)
	DirectX::XMFLOAT3 GetMoveVec_2D() const;

	// �ړ�����(3D)_�v���C���[
	void Move3D_Player(float vx, float vz, float speed, bool slide = false);

	// �ړ�����(3D)
	void Move3D(float vx, float vz, float speed);

	// �ړ�����(2D)
	void Move2D_Player(float vx, float speed, bool slide = false);

	// ���񏈗�(�[���x�N�g���ł��������~�߂Ȃ�)
	void Turn3DNotCancel(float elapsedTime, float vx, float vz, float speed);

	// ���񏈗�(3D)
	void Turn3D_Player(float elapsedTime, float vx, float vz, float speed);

	// ���񏈗�(2D)
	void Turn2D_Player();

	// �W�����v����
	void Jump(float speed);

#endif

	/**************************************************************************************/

public:

	/*! �G�֘A */
#if 1

	// �^�[�Q�b�g�ɓ����������𔻒�
	bool IsArrivalTarget();

	// �^�[�Q�b�g�̕�������
	void DirectionToTarget(float elapsedTime, float speedRate);

	// �^�[�Q�b�g�̕��ֈ꒼���ɐi��
	void MoveToDirection_3D(float speedRate);

	// �G�̔j������
	void Destroy_Enemy();

#endif
};