#pragma once

#include "Game/GameObjectBase.h"

// �O���錾
class Graphics;
class Shader;

// ��
class Sky : public GameObjectBase
{
public:
	Sky();
	~Sky() override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);
};