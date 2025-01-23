#include "Scene.h"
#include "Game/Camera/Camera.h"

/*! �f�o�b�O�p */
bool Scene::debugControll = false; // �f�o�b�O�p�ɐ��l��ύX�ł���悤�ɂ��邩
DirectX::XMFLOAT4 Scene::debugLightDirection = { 0.0f, -1.0f, 0.0f, 0.0f }; // ���C�g�̌���
DirectX::XMFLOAT4 Scene::debugLightColor = { 0.8f, 0.8f, 0.8f, 1 };     // ���C�g�̐F
DirectX::XMFLOAT3 Scene::debugLightPosition = { 0, 0, 0 };                 // ���C�g�̈ʒu

//! ���C�g�ւ̉e��
float Scene::debugAmbientStrength = 1.3f; // ����
float Scene::debugDiffuseStrength = 0.5f; // �g�U��
float Scene::debugSpecularStrength = 0.5f; // �X�y�L�����[��
float Scene::debugLightRange = 1.0f; // ���C�g�͈̔�

//! �t�H�O
DirectX::XMFLOAT3 Scene::debugFogColor = { 0.7f, 0.8f, 0.9f }; // �t�H�O�̐F
float             Scene::debugFogStart = 80.0f;                // �t�H�O�̊J�n
float             Scene::debugFogEnd = 200.0f;               // �t�H�O�̏I��

// �`��ݒ�
void Scene::DrawingSettings(Graphics& graphics)
{
	dc = graphics.GetDeviceContext();
	rtv = graphics.GetRenderTargetView();
	dsv = graphics.GetDepthStencilView();

	// ��ʃN���A&�����_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { clearColor.x, clearColor.y, clearColor.z, 1.0f }; //RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	if (!debugControll)
	{
		/*! ���C�g�̐ݒ� */
		// ���C�g�̐F
		rc.lightColor = this->debugLightColor = { lightColor.x, lightColor.y, lightColor.z, 1.0f };
		// ���C�g�̌���
		rc.lightDirection = this->debugLightDirection = { lightDirection.x, lightDirection.y, lightDirection.z, 1.0f };
		// ���C�g�̈ʒu
		rc.lightPosition = this->debugLightPosition = this->lightPosition;
		// ���C�g�͈̔�
		rc.lightRange = this->debugLightRange = this->lightRange;

		// �����A�g�U���A �X�y�L�����[���̐ݒ�
		rc.ambientStrength = this->debugAmbientStrength = this->ambientStrength;  // ����
		rc.diffuseStrength = this->debugDiffuseStrength = this->diffuseStrength;  // �g�U��
		rc.specularStrength = this->debugSpecularStrength = this->specularStrength; // �X�y�L�����[��

		/*! �t�H�O�̐ݒ� */
		// �t�H�O�̐F
		rc.fogColor = { this->fogColor.x, this->fogColor.y, this->fogColor.z, 1.0f };
		// �t�H�O�̐F(�f�o�b�O)
		this->debugFogColor = { this->fogColor.x, this->fogColor.y, this->fogColor.z };
		// �t�H�O�̊J�n
		rc.fogStart = this->debugFogStart = this->fogStart;
		// �t�H�O�̏I��
		rc.fogEnd = this->debugFogEnd = this->fogEnd;
	}
	else
	{
		/*! ���C�g�̐ݒ� */
		// ���C�g�̐F
		rc.lightColor = { this->debugLightColor.x, this->debugLightColor.y, this->debugLightColor.z, this->debugLightColor.w };
		// ���C�g�̌���
		rc.lightDirection = { this->debugLightDirection.x, this->debugLightDirection.y, this->debugLightDirection.z, this->debugLightDirection.w };
		// ���C�g�̈ʒu
		rc.lightPosition = this->debugLightPosition;
		// ���C�g�͈̔�
		rc.lightRange = this->debugLightRange;

		// �����A�g�U���A �X�y�L�����[���̐ݒ�
		rc.ambientStrength = this->debugAmbientStrength;  // ����
		rc.diffuseStrength = this->debugDiffuseStrength;  // �g�U��
		rc.specularStrength = this->debugSpecularStrength; // �X�y�L�����[��

		/*! �t�H�O�̐ݒ� */
		// �t�H�O�̐F
		rc.fogColor = { this->debugFogColor.x, this->debugFogColor.y, this->debugFogColor.z, 1.0f };
		// �t�H�O�̊J�n
		rc.fogStart = this->debugFogStart;
		// �t�H�O�̏I��
		rc.fogEnd = this->debugFogEnd;
	}

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
}