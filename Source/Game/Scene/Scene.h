#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Graphics/Graphics.h"

// �V�[��
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	// ������
	virtual void Initialize() = 0;

	// �I����
	virtual void Finalize() = 0;

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render() = 0;

	// �����������Ă��邩
	bool IsReady() const { return ready; }

	// ���������ݒ�
	void SetReady() { ready = true; }

	// �`��ݒ�
	void DrawingSettings(Graphics& graphics,
		DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f },
		float ambientStrength = 1.3f,
		float diffuseStrength = 0.5f,
		float specularStrength = 0.5f,
		float lightRange = 100.0f,
		DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f },
		float fogStart = 80.0f, float fogEnd = 200.0f);

public:
	DirectX::XMFLOAT4 lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f }; // ���C�g�̌���
	DirectX::XMFLOAT4 lightColor = { 0.8f, 0.8f, 0.8f, 1 };     // ���C�g�̐F
	DirectX::XMFLOAT3 lightPosition = { 0, 0, 0 };                 // ���C�g�̈ʒu

	/*! �f�o�b�O�p */
	static bool debugControll;                    // �f�o�b�O�p�ɐ��l��ύX�ł���悤�ɂ��邩

	static DirectX::XMFLOAT4 debugLightDirection; // ���C�g�̌���
	static DirectX::XMFLOAT4 debugLightColor;     // ���C�g�̐F
	static DirectX::XMFLOAT3 debugLightPosition;  // ���C�g�̈ʒu

	static float debugAmbientStrength; // ����
	static float debugDiffuseStrength; // �g�U��
	static float debugSpecularStrength; // �X�y�L�����[��
	static float debugLightRange;       // ���C�g�͈̔�

	static DirectX::XMFLOAT3 debugFogColor; // �t�H�O�̐F
	static float             debugFogStart;  // �t�H�O�̊J�n
	static float             debugFogEnd; // �t�H�O�̏I��

protected:
	ID3D11DeviceContext* dc;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;
	RenderContext rc;

protected:
	DirectX::XMFLOAT3 clearColorConst = { 0.5f, 0.5f, 0.5f }; // �N���A�J���[(���l�Œ�)

	const float ambientStrengthConst = 1.3f; // ����(���l�Œ�)
	const float diffuseStrengthConst = 0.5f; // �g�U��(���l�Œ�)
	const float specularStrengthConst = 0.5f; // �X�y�L�����[��(���l�Œ�)
	const float lightRangeConst = 1.0f; // ���C�g�͈̔�(���l�Œ�)

	DirectX::XMFLOAT3 fogColorConst = { 0.7f, 0.8f, 0.9f }; // �t�H�O�̐F(���l�Œ�)
	float             fogStartConst = 80.0f;                // �t�H�O�̊J�n(���l�Œ�)
	float             fogEndConst = 200.0f;               // �t�H�O�̏I��(���l�Œ�)

private:
	bool ready = false;
};