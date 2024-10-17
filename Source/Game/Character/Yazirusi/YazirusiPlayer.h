#pragma once

#include "Graphics/Model.h"
#include "Yazirusi.h"

// ’¼i’eŠÛ
class YazirusiPlayer : public Yazirusi
{
public:
	YazirusiPlayer(YazirusiManager* manage);
	~YazirusiPlayer() override;

	// XVˆ—
	void Update(float elapsedTime) override;

	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// ”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float  speed = 50.0f;
	float  lifeTimer = 1.5f;
};