#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "RenderTarget.h"

class ShadowMap
{
private:
	struct CbShadowMap
	{
		DirectX::XMFLOAT4X4 shadowProjection; // �V���h�E
		float  shadowBias;               // �V���h�E�o�C�A�X
		DirectX::XMFLOAT3 shadowPadding; // �V���h�E�̃p�f�B���O
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> CBShadowMap;

public:
	// ����������
	void Initialize();

	// �J�n����
	void Begin(const RenderContext& rc);

	// �I������
	void End();

	// �V�F�[�_�[���\�[�X�r���[���擾
	ID3D11ShaderResourceView* GetSRV() { return shadowmap->GetSRV(); }

private:
	std::unique_ptr<RenderTarget> shadowmap;
};
