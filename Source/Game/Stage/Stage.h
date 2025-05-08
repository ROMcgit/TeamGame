#pragma once

#include "Graphics/Shader.h"
#include "Other/Collision.h"
#include "Game/Character/GameObjectBase.h"

// �X�e�[�W
class Stage : public GameObjectBase
{
public:
	Stage() {}
	virtual ~Stage() {};

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// ���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	// �f�o�b�O�pGUI�`��
	virtual void DrawDebugGUI() {};

	// �X�e�[�W���폜
	void Destroy();

protected:
	float noViewTime = 0.1f;
};