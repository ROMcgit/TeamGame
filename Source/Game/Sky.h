#pragma once

#include "Game/GameObjectBase.h"

// ‘O•ûéŒ¾
class Graphics;
class Shader;

// ‹ó
class Sky : public GameObjectBase
{
public:
	Sky();
	~Sky() override;

	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader);
};