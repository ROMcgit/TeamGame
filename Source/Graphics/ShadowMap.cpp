#include "ShadowMap.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderSetting.h"

#include "Game/Camera/CameraController.h"
#include "Game/Scene/Scene.h"

// ����������
void ShadowMap::Initialize()
{
	// �����_�[�^�[�Q�b�g
	shadowmap = std::make_unique<RenderTarget>
		(Graphics::Instance().GetDevice(),
			4096, 4096, DXGI_FORMAT_R16_FLOAT, true);

	// �R���X�^���g�o�b�t�@����
	ShaderSetting::CreateConstantBuffer(
		Graphics::Instance().GetDevice(),
		sizeof(CbShadowMap),
		CBShadowMap.GetAddressOf());
}

// �J�n����
void ShadowMap::Begin(const RenderContext& rc)
{
	// slot10���͂���
	ID3D11ShaderResourceView* dummy = nullptr;
	Graphics::Instance().GetDeviceContext()->PSSetShaderResources(10, 1, &dummy);

	// �����_�[�^�[�Q�b�g�J�n
	shadowmap->Begin();

	// ���̈ʒu����̍s��𐶐�
	DirectX::XMVECTOR Eye    = { rc.lightPosition.x + Scene::shadowMapEyeOffsetStatic.x,
		                         rc.lightPosition.y + Scene::shadowMapEyeOffsetStatic.y, 
		                         rc.lightPosition.z + Scene::shadowMapEyeOffsetStatic.z };

	DirectX::XMVECTOR Target = { rc.lightPosition.x, rc.lightPosition.y, rc.lightPosition.z };
	DirectX::XMMATRIX View   = DirectX::XMMatrixLookAtLH(Eye, Target, { 0,1,0 });
	// ���e�ϊ�(���s���e)
	DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(
		Scene::shadowMapWidthStatic,    // ���͈̔�
		Scene::shadowMapLengthStatic,   // �c�͈̔�
		Scene::shadowMapStartStatic, Scene::shadowMapEndStatic  // �`�悷��ŏ��E�ő勗��
	);

	// �R���X�^���g�o�b�t�@�X�V���Z�b�g
	CbShadowMap param;
	DirectX::XMStoreFloat4x4(
		&param.shadowProjection,
		View * Projection);

	// �V���h�E�o�C�A�X
	param.shadowBias = Scene::shadowMapBiasStatic;
	// �V���h�E�p�f�B���O
	param.shadowPadding = { 0.0f, 0.0f, 0.0f };

	Graphics::Instance().GetDeviceContext()->UpdateSubresource(CBShadowMap.Get(), 0, 0, &param, 0, 0);

	// Slot10�ɃZ�b�g
	ID3D11Buffer* psConstantBuffers[] =
	{
		CBShadowMap.Get(),
	};
	Graphics::Instance().GetDeviceContext()->VSSetConstantBuffers(10, _countof(psConstantBuffers), psConstantBuffers);
	Graphics::Instance().GetDeviceContext()->PSSetConstantBuffers(10, _countof(psConstantBuffers), psConstantBuffers);
}

// �I������
void ShadowMap::End()
{
	// �����_�[�^�[�Q�b�g�̏I��
	shadowmap->End();

	// �s�N�Z���V�F�[�_�[slot10�ɃZ�b�g
	shadowmap->SetPSSlot(10);
}