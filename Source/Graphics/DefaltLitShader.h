#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class DefaltLitShader : public Shader
{
public:
	DefaltLitShader(ID3D11Device* device);
	~DefaltLitShader() override {}

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
		DirectX::XMFLOAT4	lightDirection;   // ���C�g�̌���
		DirectX::XMFLOAT4   lightColor;       // ���C�g�̐F
		DirectX::XMFLOAT3   lightPosition;    // ���C�g�̈ʒu
		float               lightRange;       // ���C�g�͈̔�

		float               ambientStrength;  // ����
		float               diffuseStrength;  // �g�U��
		float               specularStrength; // �X�y�L�����[��
		float               lightPadding;     // ���C�g�̃p�f�B���O

		DirectX::XMFLOAT4   fogColor;         // �t�H�O�̐F
		float               fogStart;         // �t�H�O�̊J�n
		float               fogEnd;           // �t�H�O�̏I��
		DirectX::XMFLOAT2   fogPadding;       // �t�H�O�̃p�f�B���O

		float               focusDistance;    // �t�H�[�J�X����
		float               focusRange;       // �t�H�[�J�X�͈�
		float               blurStrength;     // �ڂ������x
		float               focusPadding;     // �t�H�[�J�X�̃p�f�B���O
	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT3 emissiveColor;    //�G�~�b�V�u
		float             emissiveStrength; //�G�~�b�V�u�̋���
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
