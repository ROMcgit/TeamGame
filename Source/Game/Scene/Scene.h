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

	static DirectX::XMFLOAT4 debugLightColor;         // ���C�g�̐F
	static DirectX::XMFLOAT4 debugLightDirection;     // ���C�g�̌���
	static DirectX::XMFLOAT3 debugLightPosition;      // ���C�g�̈ʒu
	static DirectX::XMFLOAT3 debugShadowMapEyeOffset; // �V���h�E�}�b�v�̈ʒu�̃I�t�Z�b�g
	static float             debugShadowMapBias;      // �V���h�E�}�b�v�̃o�C�A�X
	static float             debugShadowMapWidth;     // �V���h�E�}�b�v�̉��͈̔�
	static float             debugShadowMapLength;    // �V���h�E�}�b�v�̏c�͈̔�
	static float             debugShadowMapStart;     // �V���h�E�}�b�v�̊J�n
	static float             debugShadowMapEnd;       // �V���h�E�}�b�v�̏I��

	static float debugAmbientStrength; // ����
	static float debugDiffuseStrength; // �g�U��
	static float debugSpecularStrength; // �X�y�L�����[��
	static float debugLightRange;       // ���C�g�͈̔�

	static DirectX::XMFLOAT3 debugFogColor; // �t�H�O�̐F
	static float             debugFogStart; // �t�H�O�̊J�n
	static float             debugFogEnd;   // �t�H�O�̏I��

protected:
	ID3D11DeviceContext* dc; // �f�o�C�X�R���e�L�X�g
	ID3D11RenderTargetView* rtv; // �����_�[�^�[�Q�b�g�r���[
	ID3D11DepthStencilView* dsv; // �f�v�X�X�e���V���r���[
	RenderContext            rc; // �����_�[�R���e�L�X�g

public:
	/*! �V���h�E�}�b�v�N���X�Ŏg�� */
	static DirectX::XMFLOAT3 shadowMapEyeOffsetStatic; // �V���h�E�}�b�v�̈ʒu�̃I�t�Z�b�g
	static float shadowMapBiasStatic;                  // �V���h�E�}�b�v�̃o�C�A�X
	static float shadowMapWidthStatic;                 // �V���h�E�}�b�v�̉��͈̔�
	static float shadowMapLengthStatic;                // �V���h�E�}�b�v�̏c�͈̔�
	static float shadowMapStartStatic;                 // �V���h�E�}�b�v�̊J�n
	static float shadowMapEndStatic;                   // �V���h�E�}�b�v�̏I��

protected:

	DirectX::XMFLOAT3 lightColor = { 0.8f, 0.8f, 0.8f };  // ���C�g�̐F
	DirectX::XMFLOAT3 lightDirection = { 0.3f, -1.0f, 0.0f }; // ���C�g�̌���
	DirectX::XMFLOAT3 lightPosition = { 0.0f, 0.0f, 0.0f };  // ���C�g�̈ʒu
	float             lightRange = 2000.0f;               // ���C�g�͈̔�
	DirectX::XMFLOAT3 shadowMapEyeOffset = { -1.7f, 30.0f, -9.5f };  // �V���h�E�}�b�v�̈ʒu�̃I�t�Z�b�g
	float             shadowMapBias = 0.005f;                // �V���h�E�}�b�v�̃o�C�A�X
	float             shadowMapWidth = 100.0f;                // �V���h�E�}�b�v�̉��͈̔�
	float             shadowMapLength = 100.0f;                // �V���h�E�}�b�v�̏c�͈̔�
	float             shadowMapStart = 1.0f;                  // �V���h�E�}�b�v�̊J�n
	float             shadowMapEnd = 100.0f;                // �V���h�E�}�b�v�̏I��

	DirectX::XMFLOAT3 clearColor = { 0.5f, 0.5f, 0.5f }; // �N���A�J���[

	float ambientStrength = 1.3f; // ����
	float diffuseStrength = 0.5f; // �g�U��
	float specularStrength = 0.5f; // �X�y�L�����[��

	DirectX::XMFLOAT3 fogColor = { 0.7f, 0.8f, 0.9f }; // �t�H�O�̐F
	float             fogStart = 80.0f;                // �t�H�O�̊J�n
	float             fogEnd = 200.0f;               // �t�H�O�̏I��

public:
	static float             contrastStatic;            // �R���g���X�g
	static float             saturationStatic;          // �F�̍ʓx(���l�Œ�)
	static DirectX::XMFLOAT3 colorFilterStatic;         // �J���[�t�B���^�[(���l�Œ�)
	static float             chromaticAberrationStatic; // �F����(���l�Œ�)
	static float             hueShiftStatic;            // �F���V�t�g

	static float vignetteIntensityStatic; // �r�l�b�g�̋��x
	static float vignetteOpacityStatic;   // �r�l�b�g�̕s�����x
	static float blurStrengthStatic;      // �u���[�̋��x
	static float bloomThresholdStatic;    // �O���A��臒l
	static float bloomIntensityStatic;    // �O���A�̋��x

	static float focusDistanceStatic;     // �s���g�����킹�鋗��
	static float focusRangeStatic;        // �s���g�������͈�
	static float dofBlurStrengthStatic;   // ��ʊE�[�x�̃u���[���x

protected:
	float contrast = 1.0f;                  // �R���g���X�g
	float saturation = 0.8f;                  // �F�̍ʓx
	DirectX::XMFLOAT3 colorFilter = { 1.2f, 1.3f, 1.35f }; // �J���[�t�B���^�[
	float chromaticAberration = 0.0f;                  // �F����
	float hueShift = 0.0f;                  // �F���V�t�g

	float vignetteIntensity = 0.0f; // �r�l�b�g�̋��x
	float vignetteOpacity = 0.0f; // �r�l�b�g�̕s�����x
	float blurStrength = 0.0f; // �u���[�̋��x
	float bloomThreshold = 1.3f; // �O���A��臒l
	float bloomIntensity = 1.0f; // �O���A�̋��x

	float focusDistance = 0.3f;  // �s���g�����킹�鋗��
	float focusRange = 3.0f;  // �s���g�������͈�
	float dofBlurStrength = 15.0f; // ��ʊE�[�x�̃u���[���x

private:
	bool ready = false;
};