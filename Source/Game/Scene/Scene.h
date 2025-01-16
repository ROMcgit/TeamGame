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
	static DirectX::XMFLOAT4 lightDirection; // ���C�g�̌���
	static DirectX::XMFLOAT4 lightColor;     // ���C�g�̐F
	static DirectX::XMFLOAT3 lightPosition;  // ���C�g�̈ʒu

protected:
	ID3D11DeviceContext* dc;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;
	RenderContext rc;

protected:
	DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f }; // �N���A�J���[
	float ambientStrength = 1.3f; // ����
	float diffuseStrength = 0.5f; // �g�U��
	float specularStrength = 0.5f; // �X�y�L�����[��
	float lightRange = 1.0f; // ���C�g�͈̔�

	DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f }; // �t�H�O�̐F
	float fogStart = 80.0f;  // �t�H�O�̊J�n
	float fogEnd = 200.0f; // �t�H�O�̏I��

private:
	bool ready = false;
};