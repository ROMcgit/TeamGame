#pragma once

#include "Game/Character/GameObjectBase.h"

// 前方宣言
class Graphics;
class Shader;

// 空
class Sky : public GameObjectBase
{
public:
	Sky();
	~Sky() override;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);
};