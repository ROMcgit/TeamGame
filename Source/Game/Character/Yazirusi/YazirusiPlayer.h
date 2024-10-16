#pragma once

#include "Graphics/Model.h"
#include "Yazirusi.h"

// ���i�e��
class YazirusiPlayer : public Yazirusi
{
public:
	YazirusiPlayer(YazirusiManager* manage);
	~YazirusiPlayer() override;

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
};