#include "Scene.h"
#include "Game/Camera/Camera.h"

/*! �f�o�b�O�p */
bool Scene::debugControll = false;                       // �f�o�b�O�p�ɐ��l��ύX�ł���悤�ɂ��邩
//! ���C�g
DirectX::XMFLOAT4 Scene::debugLightDirection = { 0.0f, -1.0f, 0.0f, 0.0f }; // ���C�g�̌���
DirectX::XMFLOAT4 Scene::debugLightColor = { 0.8f, 0.8f, 0.8f, 1 };     // ���C�g�̐F
DirectX::XMFLOAT3 Scene::debugLightPosition = { 0.0f, 0.0f, 0.0f };        // ���C�g�̈ʒu

//! �V���h�E�}�b�v
DirectX::XMFLOAT3 Scene::debugShadowMapEyeOffset = { 0.0f, 0.0f, 0.0f }; //! �V���h�E�}�b�v�̈ʒu�̃I�t�Z�b�g
float Scene::debugShadowMapBias = 0;                                    // �V���h�E�}�b�v�̃o�C�A�X
float Scene::debugShadowMapWidth = 0;                                    // �V���h�E�}�b�v�̉��͈̔�
float Scene::debugShadowMapLength = 0;                                    // �V���h�E�}�b�v�̏c�͈̔�
float Scene::debugShadowMapStart = 0;                                    // �V���h�E�}�b�v�̊J�n
float Scene::debugShadowMapEnd = 0;                                    // �V���h�E�}�b�v�̏I��

//! �V���h�E�}�b�v�N���X�Ŏg��
DirectX::XMFLOAT3 Scene::shadowMapEyeOffsetStatic = { 0.0f, 0.0f, 0.0f }; //! �V���h�E�}�b�v�̈ʒu�̃I�t�Z�b�g
float Scene::shadowMapBiasStatic = 0;                                   // �V���h�E�}�b�v�̃o�C�A�X
float Scene::shadowMapWidthStatic = 0;                                   // �V���h�E�}�b�v�̉��͈̔�
float Scene::shadowMapLengthStatic = 0;                                   // �V���h�E�}�b�v�̏c�͈̔�
float Scene::shadowMapStartStatic = 0;                                   // �V���h�E�}�b�v�̊J�n
float Scene::shadowMapEndStatic = 0;                                   // �V���h�E�}�b�v�̏I��

//! ���C�g�ւ̉e��
float Scene::debugAmbientStrength = 1.3f; // ����
float Scene::debugDiffuseStrength = 0.5f; // �g�U��
float Scene::debugSpecularStrength = 0.5f; // �X�y�L�����[��
float Scene::debugLightRange = 1.0f; // ���C�g�͈̔�

//! �t�H�O
DirectX::XMFLOAT3 Scene::debugFogColor = { 0.7f, 0.8f, 0.9f }; // �t�H�O�̐F
float             Scene::debugFogStart = 80.0f;                // �t�H�O�̊J�n
float             Scene::debugFogEnd = 200.0f;               // �t�H�O�̏I��

/*! �|�X�g�G�t�F�N�g */
//! �R���g���X�g
float Scene::contrastStatic = 0.0f;
//! �T�`�����[�V����
float Scene::saturationStatic = 0.0f;
//! �J���[�t�B���^�[
DirectX::XMFLOAT3 Scene::colorFilterStatic = { 0.0f, 0.0f, 0.0f };
//! �N���}�e�B�b�N�A�x���[�V����
float Scene::chromaticAberrationStatic = 0.0f;
//! �F���V�t�g
float Scene::hueShiftStatic = 0.0f;

//! �r�l�b�g�̋��x
float Scene::vignetteIntensityStatic = 0.0f;
//! �r�l�b�g�̕s�����x
float Scene::vignetteOpacityStatic = 0.0f;
//! �u���[�̋��x
float Scene::blurStrengthStatic = 0.0f;
//! �O���A��臒l
float Scene::bloomThresholdStatic = 0.0f;
//! �O���A�̋��x
float Scene::bloomIntensityStatic = 0.0f;

//! �s���g�����킹�鋗��
float Scene::focusDistanceStatic = 0.0f;
//! �s���g�������͈�
float Scene::focusRangeStatic = 0.0f;
//! ��ʊE�[�x�̃u���[���x
float Scene::dofBlurStrengthStatic = 0.0f;

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
		rc.lightColor = this->debugLightColor = { this->lightColor.x, this->lightColor.y, this->lightColor.z, 1.0f };
		// ���C�g�̌���
		rc.lightDirection = this->debugLightDirection = { this->lightDirection.x, this->lightDirection.y, this->lightDirection.z, 1.0f };
		// ���C�g�̈ʒu
		rc.lightPosition = this->debugLightPosition = this->lightPosition;
		// ���C�g�͈̔�
		rc.lightRange = this->debugLightRange = this->lightRange;

		//! �V���h�E�}�b�v
		// �V���h�E�}�b�v�̈ʒu�̃I�t�Z�b�g
		shadowMapEyeOffsetStatic = this->debugShadowMapEyeOffset = this->shadowMapEyeOffset;
		// �V���h�E�}�b�v�̃o�C�A�X
		shadowMapBiasStatic = this->debugShadowMapBias = this->shadowMapBias;
		// �V���h�E�}�b�v�̉��͈̔�
		shadowMapWidthStatic = this->debugShadowMapWidth = this->shadowMapWidth;
		// �V���h�E�}�b�v�̏c�͈̔�
		shadowMapLengthStatic = this->debugShadowMapLength = this->shadowMapLength;
		// �V���h�E�}�b�v�̊J�n
		shadowMapStartStatic = this->debugShadowMapStart = this->shadowMapStart;
		// �V���h�E�}�b�v�̏I��
		shadowMapEndStatic = this->debugShadowMapEnd = this->shadowMapEnd;

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


		/*! �|�X�g�G�t�F�N�g */
		// �R���g���X�g
		contrastStatic = this->contrast;
		// �T�`�����[�V����
		saturationStatic = this->saturation;
		// �J���[�t�B���^�[
		colorFilterStatic = this->colorFilter;
		// �N���}�e�B�b�N�A�x���[�V����
		chromaticAberrationStatic = this->chromaticAberration;
		// �F���V�t�g
		hueShiftStatic = this->hueShift;

		// �r�l�b�g�̋��x
		vignetteIntensityStatic = this->vignetteIntensity;
		// �r�l�b�g�̕s�����x
		vignetteOpacityStatic = this->vignetteOpacity;
		// �u���[�̋��x
		blurStrengthStatic = this->blurStrength;
		// �O���A��臒l
		bloomThresholdStatic = this->bloomThreshold;
		// �O���A�̋��x
		bloomIntensityStatic = this->bloomIntensity;

		// �s���g�����킹�鋗��
		focusDistanceStatic = this->focusDistance;
		// �s���g�������͈�
		focusRangeStatic = this->focusRange;
		// ��ʊE�[�x�̃u���[���x
		dofBlurStrengthStatic = this->dofBlurStrength;
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

		//! �V���h�E�}�b�v
		// �V���h�E�}�b�v�̈ʒu�̃I�t�Z�b�g
		shadowMapEyeOffsetStatic = this->debugShadowMapEyeOffset;
		// �V���h�E�}�b�v�̃o�C�A�X
		shadowMapBiasStatic = this->debugShadowMapBias;

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