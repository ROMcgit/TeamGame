#pragma once

#include <wrl.h>
#include <d3d11.h>
#include "Graphics.h"
#include "Graphics/HDRTexture.h"

// 描画ターゲットバッファ
class RenderTarget
{
	// 描画関連
public:
	void Render();

private:
	// 頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT2	texcoord;
	};
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>  samplerState;

public:
	// 開始処理
	void Begin();

	// 終了処理
	void End();

	// ピクセルシェーダーのスロットを設定
	void SetPSSlot(int slot);

public:
	RenderTarget(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format, bool depth = false);
	~RenderTarget();

	// シェーダーリソースビュー取得
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() { return shaderResourceView; }

	// レンダーターゲットビュー取得
	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return renderTargetView; }

	// テクスチャの情報取得
	D3D11_TEXTURE2D_DESC GetTexture2dDesc() { return texture2dDesc; }

	// テクスチャ幅取得
	inline int GetWidth() const { return texture2dDesc.Width; }

	// テクスチャ高さ取得
	inline int GetHeight() const { return texture2dDesc.Height; }

	// シェーダーリソースビューを取得
	ID3D11ShaderResourceView* GetSRV() { return shaderResourceView.Get(); }

	// 環境マップを取得
	HDRTexture* GetEnvironmentMap() { return environmentMap.get(); }

public:
	ID3D11RenderTargetView* orgRtv[4];
	ID3D11DepthStencilView* orgDsv;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	 depthStencilView;
	D3D11_TEXTURE2D_DESC							 texture2dDesc;

	std::unique_ptr<HDRTexture> environmentMap; // 環境マップ
};

