#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// �X�e�[�W_����
class G5_StageAsibawatari_Normal_Horizontal : public Stage
{
public:
	G5_StageAsibawatari_Normal_Horizontal();
	~G5_StageAsibawatari_Normal_Horizontal();

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// �f�o�b�OGUI
	void DrawDebugGUI() override;

private:
	// �ړ��X�V����
	void UpdateMove(float elapsedTime);

private:
	std::unique_ptr<Model> model;
};