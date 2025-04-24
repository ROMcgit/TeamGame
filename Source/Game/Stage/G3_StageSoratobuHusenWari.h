#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

// �X�e�[�W
class G3_StageSoratobuHusenWari : public Stage
{
public:
	G3_StageSoratobuHusenWari();
	~G3_StageSoratobuHusenWari();

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	// �f�o�b�OGUI
	void DrawDebugGUI() override;

	//! �ʒuZ�̏������
	void SetPositionZInit(float positionZInit) { this->positionZInit = positionZInit; }

private:
	// �ʒu�X�V����
	void UpdatePosition(float elapsedTime);

private:
	std::unique_ptr<Model> model;

	float positionZInit = 0.0f; //�@�����̈ʒuZ
};