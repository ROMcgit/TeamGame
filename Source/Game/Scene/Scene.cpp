#include "Scene.h"
#include "Game/Camera/Camera.h"

DirectX::XMFLOAT4 Scene::lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f }; // ���C�g�̌���
DirectX::XMFLOAT4 Scene::lightColor = { 0.8f, 0.8f, 0.8f, 1 };     // ���C�g�̐F
DirectX::XMFLOAT3 Scene::lightPosition = { 0, 0, 0 };                 // ���C�g�̈ʒu

// �`��ݒ�
void Scene::DrawingSettings(Graphics& graphics,
	DirectX::XMFLOAT3 clearColor,
	float ambientStrength,
	float diffuseStrength,
	float specularStrength,
	float lightRange,
	DirectX::XMFLOAT3 fogColor,
	float fogStart, float fogEnd)
{
	dc = graphics.GetDeviceContext();
	rtv = graphics.GetRenderTargetView();
	dsv = graphics.GetDepthStencilView();

	// ��ʃN���A&�����_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { clearColor.x, clearColor.y, clearColor.z, 1.0f }; //RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// ���C�g�̐ݒ�
	rc.lightColor = { this->lightColor.x, this->lightColor.y, this->lightColor.z, this->lightColor.w };
	rc.lightDirection = { this->lightDirection.x, this->lightDirection.y, this->lightDirection.z, this->lightDirection.w }; // ���C�g�����i�������j
	rc.lightPosition = this->lightPosition;
	rc.lightRange = lightRange;

	// �����A�g�U���A �X�y�L�����[���̐ݒ�
	rc.ambientStrength = ambientStrength;
	rc.diffuseStrength = diffuseStrength;
	rc.specularStrength = specularStrength;

	// �t�H�O�̐ݒ�
	rc.fogColor = { fogColor.x, fogColor.y, fogColor.z, 1.0f }; // �t�H�O�̐F
	rc.fogStart = fogStart;                                     // �t�H�O�̊J�n
	rc.fogEnd   = fogEnd;                                       // �t�H�O�̏I��

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
}