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
	void DrawingSettings(Graphics& graphics);

public:
	/*! �f�o�b�O�p */
	static bool debugControll; // �f�o�b�O�p�ɐ��l��ύX�ł���悤�ɂ��邩

	static DirectX::XMFLOAT4 debugLightColor;     // ���C�g�̐F
	static DirectX::XMFLOAT4 debugLightDirection; // ���C�g�̌���
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

	DirectX::XMFLOAT3 lightColor = { 0.8f, 0.8f, 0.8f };  // ���C�g�̐F
	DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f }; // ���C�g�̌���
	DirectX::XMFLOAT3 lightPosition = { 0, 0, 0 };           // ���C�g�̈ʒu
	float lightRange = 100.0f;

	DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f }; // �N���A�J���[

	float ambientStrength = 1.3f; // ����
	float diffuseStrength = 0.5f; // �g�U��
	float specularStrength = 0.5f; // �X�y�L�����[��

	DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f }; // �t�H�O�̐F
	float             fogStart = 80.0f;                // �t�H�O�̊J�n
	float             fogEnd = 200.0f;               // �t�H�O�̏I��

	const float contrastConst = 1.0f;                  // �R���g���X�g(���l�Œ�)
	const float saturationConst = 0.8f;                  // �F�̍ʓx(���l�Œ�)
	const DirectX::XMFLOAT3 colorFilterConst = { 1.2f, 1.3f, 1.35f }; // �J���[�t�B���^�[(���l�Œ�)
	const float chromaticAberrationConst = 0.0f;                  // �F����(���l�Œ�)

private:
	bool ready = false;
};