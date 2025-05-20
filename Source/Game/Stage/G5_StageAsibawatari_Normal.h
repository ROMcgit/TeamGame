#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// �X�e�[�W
class G5_StageAsibawatari_Normal : public Stage
{
public:
	G5_StageAsibawatari_Normal();
	~G5_StageAsibawatari_Normal();

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// �f�o�b�OGUI
	void DrawDebugGUI() override;

	//
	void SetMoveDown() { this->moveDown = true; }

private:
	// �ړ�����
	void UpdateMove(float elapsedTime);

private:
	std::unique_ptr<Model> model;

	bool moveDown = false;
	float moveTimer = 0.0f;
};