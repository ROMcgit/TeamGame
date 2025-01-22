#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// �X�e�[�W
class G0_StageOnigokko : public Stage
{
public:
	G0_StageOnigokko();
	~G0_StageOnigokko();

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	// �s��X�V����
	void UpdateTransform();

private:
	std::unique_ptr<Model> model;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT4X4 oldTransform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };

	DirectX::XMFLOAT3 torque = { 0,0,0 };
	DirectX::XMFLOAT3 start = { 0,0,0 };
	DirectX::XMFLOAT3 goal = { 0,0,0 };

	float moveSpeed = 2.0f;
	float moveRate = 0.0f;

	DirectX::XMFLOAT3 oldAngle = { 0,0,0 };
};