#include <filesystem>
#include <wrl.h>
#include "Other/Misc.h"
#include "ShaderSetting.h"

// ���_�V�F�[�_�[�ǂݍ���
HRESULT ShaderSetting::LoadVertexShader(
	ID3D11Device* device,
	const char* filename,
	const D3D11_INPUT_ELEMENT_DESC inputElementDescs[],
	UINT inputElementCount,
	ID3D11InputLayout** inputLayout,
	ID3D11VertexShader** vertexShader)
{
	// �t�@�C�����J��
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "Vertex Shader File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	// ���_�V�F�[�_�[����
	HRESULT hr = device->CreateVertexShader(data.get(), size, nullptr, vertexShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// ���̓��C�A�E�g
	if (inputLayout != nullptr)
	{
		hr = device->CreateInputLayout(inputElementDescs, inputElementCount, data.get(), size, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	return hr;
}

// �s�N�Z���V�F�[�_�[�ǂݍ���
HRESULT ShaderSetting::LoadPixelShader(
	ID3D11Device* device,
	const char* filename,
	ID3D11PixelShader** pixelShader)
{
	// �t�@�C�����J��
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "Pixel Shader File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	std::unique_ptr<u_char[]> data = std::make_unique<u_char[]>(size);
	fread(data.get(), size, 1, fp);
	fclose(fp);

	// �s�N�Z���V�F�[�_�[����
	HRESULT hr = device->CreatePixelShader(data.get(), size, nullptr, pixelShader);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}

// �_�~�[�e�N�X�`���쐬
HRESULT ShaderSetting::CreateDummyTexture(
	ID3D11Device* device,
	UINT color,
	ID3D11ShaderResourceView** shaderResourceView,
	D3D11_TEXTURE2D_DESC* texture2dDesc)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = &color;
	data.SysMemPitch = desc.Width;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
	HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	hr = device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// �e�N�X�`�����擾
	if (texture2dDesc != nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		(*shaderResourceView)->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		texture2d->GetDesc(texture2dDesc);
	}

	return hr;
}

// �萔�o�b�t�@�쐬
HRESULT ShaderSetting::CreateConstantBuffer(
	ID3D11Device* device,
	UINT bufferSize,
	ID3D11Buffer** constantBuffer)
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = bufferSize;
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, constantBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return hr;
}
