#pragma once

#include "Graphics/Shader.h"
#include "Game/Character/Character.h"
#include "Graphics/Graphics.h"

// �G�l�~�[
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// 2D�X�v���C�g�̕`�揈��
	virtual void SpriteRender(ID3D11DeviceContext* dc, Graphics& graphics) {};

	// �f�o�b�O�pGUI�`��
	virtual void DrawDebugGUI() {};

	// �f�o�b�O�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	void Destroy();

	void Destroy1();

	void Destroy2();

	void Destroy3();

	// �^�[�Q�b�g�̕�������
	void DirectionToTarget(float elapsedTime, float speedRate);

	// �ڕW�n�_�����Ȃ���ړ�
	void MoveToTarget(float elapsedTime, float speedRate);

protected:
	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };     // �^�[�Q�b�g�̈ʒu
	float moveSpeed = 1.0f;                             // �ړ����x
	float turnSpeed = DirectX::XMConvertToRadians(360); // ��]���x
};