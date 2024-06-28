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

	// •`‰æŠJn
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

	// •`‰æ
	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;

	// •`‰æI—¹
	virtual void End(ID3D11DeviceContext* context) = 0;
};