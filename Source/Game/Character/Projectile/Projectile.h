#pragma once

#include "Graphics/Shader.h"
#include "Game/Effect/Effect.h"
#include "../GameObjectBase.h"
#include <stdexcept>
#include <algorithm>

// �O���錾
class ProjectileManager;

// �e��
class Projectile :public GameObjectBase
{
public:
	Projectile(ProjectileManager* manager);
	virtual ~Projectile() {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// �f�o�b�O�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	// �G�t�F�N�g�̃n���h�����擾
	virtual Effekseer::Handle GetHandle() { return -1; }

	// �j��
	void Destroy();

	/*****************************************************************************************************************************/
		/*! �Q�b�^�[ */
#if 1

	// �����擾
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }

	//-----------------------------------------------------------------------------//

		// �s�����x��ς��Ă��邩���擾
	bool GetOpacityChange() { return opacityChange; }

	// �G�~�b�V�u�̐F��ς��Ă��邩���擾
	bool GetEmissiveColorChange() { return emissiveColorChange; }

	// �G�~�b�V�u�̋�����ς��Ă��邩���擾
	bool GetEmissiveStrengthChange() { return emissiveStrengthChange; }

#endif

protected:
	ProjectileManager* manager = nullptr;

	float lifeTimer = 1.0f; // ��������
};