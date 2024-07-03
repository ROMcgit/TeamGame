#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"

// ’¼i’eŠÛ
class ProjectileStraight : public Projectile
{
public:
	/*ProjectileStraight();*/
	ProjectileStraight(ProjectileManager* manage);
	~ProjectileStraight() override;

	// XVˆ—
	void Update(float elapsedTime) override;

	// •`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// ”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float  speed = 1.0f;
	float  lifeTimer = 20.0f;

	bool moveUp = false;
};