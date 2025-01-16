#pragma once

#include <DirectXMath.h>

// �����_�[�R���e�L�X�g
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	DirectX::XMFLOAT4X4	 view;
	DirectX::XMFLOAT4X4	 projection;
	DirectX::XMFLOAT4	 lightDirection;   // ���C�g�̌���
	DirectX::XMFLOAT4    lightColor;       // ���C�g�̐F
	DirectX::XMFLOAT3    lightPosition;    // ���C�g�̈ʒu
	float                lightRange;       // ���C�g�͈̔�

	float                ambientStrength;  // ����
	float                diffuseStrength;  // �g�U��
	float                specularStrength; // �X�y�L�����[��

	DirectX::XMFLOAT4    fogColor;         // �t�H�O�̐F
	float                fogStart;         // �t�H�O�̊J�n
	float                fogEnd;           // �t�H�O�̏I��

	float                focusDistance; // �t�H�[�J�X����
	float                focusRange;    // �t�H�[�J�X�͈�
	float                blurStrength;  // �ڂ������x
};
