#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Graphics/Shader.h"
#include "Graphics/DebugRenderer.h"
#include "Graphics/LineRenderer.h"
#include "Graphics/ImGuiRenderer.h"
#include "Graphics/HDRTexture.h"
#include "Graphics/DefaltLitShader.h"
#include "Graphics/ShadowMapShader.h"
#include <mutex>

// �O���t�B�b�N�X
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	// �C���X�^���X�擾
	static Graphics& Instance() { return *instance; }

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() const { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// �u�����h�X�e�[�g�̃Q�b�^�[
	ID3D11BlendState* GetBlendState() const { return blendState.Get(); }

	// �f�v�X�X�e���V���X�e�[�g�̃Q�b�^�[
	ID3D11DepthStencilState* GetDepthStencilState() const { return depthStencilState.Get(); }

	// �[�x����
	ID3D11DepthStencilState* GetDepthDisabledState() const { return depthDisabledState.Get(); }

	// �[�x�L��
	ID3D11DepthStencilState* GetDepthEnabledState() const { return depthEnabledState.Get(); }

	// ShadowMap�擾
	Shader* GetShadowMapShader() const { return shadowMapShader.get(); }

	// DefaultLitShader�擾
	Shader* GetDefaultLitShader() const { return defaultLitshader.get(); }

	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight; }

	// �f�o�b�O�����_���擾
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

	// ���C�������_���擾
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	// ImGui�����_���擾
	ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }

	// �~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex; }

private:
	static Graphics* instance;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11BlendState>        blendState;         // �u�����h�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;  // �f�v�X�X�e���V���X�e�[�g

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthDisabledState; // �[�x����
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthEnabledState;  // �[�x�L��

	std::unique_ptr<Shader>                         shadowMapShader; // �V���h�E�}�b�v�V�F�[�_�[
	std::unique_ptr<Shader>                         defaultLitshader; // �f�t�H���g���b�g�V�F�[�_�[
	std::unique_ptr<DebugRenderer>					debugRenderer;
	std::unique_ptr<LineRenderer>					lineRenderer;
	std::unique_ptr<ImGuiRenderer>					imguiRenderer;

	std::unique_ptr<HDRTexture>                     environmentMap; // HDR�e�N�X�`��

	float	screenWidth;
	float	screenHeight;

	std::mutex mutex;
};