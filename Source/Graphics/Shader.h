#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/RenderContext.h"
#include "Graphics/Model.h"

class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// ï`âÊäJén
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	// ï`âÊ
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model, DirectX::XMFLOAT3 materialColor = { 1.0f, 1.0f, 1.0f }, float opacity = 1.0f,
		DirectX::XMFLOAT3 emissiveColor = { 0.0f, 0.0f, 0.0f }, float emissiveStrength = 0.0f) = 0;

	// ï`âÊèIóπ
	virtual void End(ID3D11DeviceContext* dc) = 0;
};