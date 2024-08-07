#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"

// �ǔ��e��
class ProjectilePlayer : public Projectile
{
public:
	ProjectilePlayer(ProjectileManager* manager);
	~ProjectilePlayer() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// ����
	void Launch(const DirectX::XMFLOAT3& direction,
				const DirectX::XMFLOAT3& position);
private:
	std::unique_ptr<Model> model;
	DirectX::XMFLOAT3 target = { 0, 0, 0, };
	float moveSpeed = 3.0f;
	float turnSpeed = DirectX::XMConvertToRadians(180);
	float lifeTimer = 20.0f;
};