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

// グラフィックス
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	// インスタンス取得
	static Graphics& Instance() { return *instance; }

	// デバイス取得
	ID3D11Device* GetDevice() const { return device.Get(); }

	// デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

	// スワップチェーン取得
	IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// ブレンドステートのゲッター
	ID3D11BlendState* GetBlendState() const { return blendState.Get(); }

	// デプスステンシルステートのゲッター
	ID3D11DepthStencilState* GetDepthStencilState() const { return depthStencilState.Get(); }

	/*! サンプラーステート */
#if 1 

	// サンプラーステート(シェーダー)の取得
	ID3D11SamplerState* GetSamplerState_Shader() { return samplerState_Shader.Get(); }

	// サンプラーステート(シェーダー)のアドレス取得
	ID3D11SamplerState** GetSamplerStateAddressOf_Shader() { return samplerState_Shader.GetAddressOf(); }

	// サンプラーステート(シャドウマップ)の取得
	ID3D11SamplerState* GetSamplerState_ShadowMap() { return samplerState_ShadowMap.Get(); }

	// サンプラーステート(シャドウマップ)のアドレス取得
	ID3D11SamplerState** GetSamplerStateAddressOf_ShadowMap() { return samplerState_ShadowMap.GetAddressOf(); }

	// サンプラーステート(環境マップ)の取得
	ID3D11SamplerState* GetSamplerState_EnvironmentMap() { return samplerState_EnvironmentMap.Get(); }

	// サンプラーステート(環境マップ)のアドレス取得
	ID3D11SamplerState** GetSamplerStateAddressOf_EnvironmentMap() { return samplerState_EnvironmentMap.GetAddressOf(); }

#endif

	// 深度無効
	ID3D11DepthStencilState* GetDepthDisabledState() const { return depthDisabledState.Get(); }

	// 深度有効
	ID3D11DepthStencilState* GetDepthEnabledState() const { return depthEnabledState.Get(); }

	// ShadowMap取得
	Shader* GetShadowMapShader() const { return shadowMapShader.get(); }

	// DefaultLitShader取得
	Shader* GetDefaultLitShader() const { return defaultLitshader.get(); }

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

	// デバッグレンダラ取得
	DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

	// ラインレンダラ取得
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	// ImGuiレンダラ取得
	ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }

	// ミューテックス取得
	std::mutex& GetMutex() { return mutex; }

private:
	static Graphics* instance;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11BlendState>        blendState;         // ブレンドステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;  // デプスステンシルステート

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthDisabledState; // 深度無効
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthEnabledState;  // 深度有効

	std::unique_ptr<Shader>                         shadowMapShader; // シャドウマップシェーダー
	std::unique_ptr<Shader>                         defaultLitshader; // デフォルトリットシェーダー
	std::unique_ptr<DebugRenderer>					debugRenderer;
	std::unique_ptr<LineRenderer>					lineRenderer;
	std::unique_ptr<ImGuiRenderer>					imguiRenderer;

	std::unique_ptr<HDRTexture>                     environmentMap; // HDRテクスチャ

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState_Shader;         // サンプラーステート(シェーダー)
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState_ShadowMap;      // サンプラーステート(シャドウマップ)
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState_EnvironmentMap; // サンプラーステート(環境マップ)

	float	screenWidth;
	float	screenHeight;

	std::mutex mutex;
};