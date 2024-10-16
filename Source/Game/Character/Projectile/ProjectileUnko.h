#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"
#include "Game/Effect/Effect.h"

// ���i�e��
class ProjectileUnko : public Projectile
{
public:
	ProjectileUnko(ProjectileManager* manage);
	~ProjectileUnko() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// ����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float  speed = 50.0f;
	float  lifeTimer = 1.5f;

	std::unique_ptr<Effect> destoryEffect;
};