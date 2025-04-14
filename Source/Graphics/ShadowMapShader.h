#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class ShadowMapShader : public Shader
{
public:
	ShadowMapShader(ID3D11Device* device);
	~ShadowMapShader() override {}

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	void Update(ID3D11DeviceContext* dc, const ModelResource::Material& material);
	void Draw(ID3D11DeviceContext* dc, const Model* model, DirectX::XMFLOAT3 materialColor = { 1.0f, 1.0f, 1.0f }, float opacity = 1.0f,
		DirectX::XMFLOAT3 emissiveColor = { 0.0f,0.0f,0.0f }, float emissiveStrength = 0.0f) override;
	void End(ID3D11DeviceContext* dc) override;

private:
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	lightDirection;   // ライトの向き
		DirectX::XMFLOAT4   lightColor;       // ライトの色
		DirectX::XMFLOAT3   lightPosition;    // ライトの位置
		float               lightRangeConst;       // ライトの範囲

		float               ambientStrengthConst;  // 環境光
		float               diffuseStrengthConst;  // 拡散光
		float               specularStrengthConst; // スペキュラー光
		float               lightPadding;     // ライトのパディング

		DirectX::XMFLOAT4   fogColorConst;         // フォグの色
		float               fogStartConst;         // フォグの開始
		float               fogEndConst;           // フォグの終了
		DirectX::XMFLOAT2   fogPadding;       // フォグのパディング

		float               focusDistance;    // フォーカス距離
		float               focusRange;       // フォーカス範囲
		float               blurStrength;     // ぼかし強度
		float               focusPadding;     // フォーカスのパディング
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT3 emissiveColor;    //エミッシブ
		float             emissiveStrength; //エミッシブの強さ
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
};
