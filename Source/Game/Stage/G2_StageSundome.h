#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// �X�e�[�W
class G2_StageSundome : public Stage
{
public:
	G2_StageSundome();
	~G2_StageSundome();

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// �f�o�b�OGUI
	void DrawDebugGUI() override;

private:
	// �s��X�V����
	void UpdateTransform();

private:
	std::unique_ptr<Model> model;
};