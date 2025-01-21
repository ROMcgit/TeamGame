#pragma once

#include "Graphics/Model.h"
#include "Game/Character/Director/SettingsDirector/Director.h"

// �J�����̃^�[�Q�b�g
class CameraTarget : public Director
{
public:
	CameraTarget(DirectorManager* manager);
	~CameraTarget() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// �f�o�b�O�pGUI�`��
	void DrawDebugGUI() override;

	// ��Ԑݒ�
	void SetState(const DirectX::XMFLOAT3 direction,
		const DirectX::XMFLOAT3& position,
		float moveSpeed = 25.0f);

private:
	// �^�[�Q�b�g�̈ʒu�̏�ԍX�V����
	bool UpdateTargetState();

private:
	std::unique_ptr<Model> model;
	float moveSpeed = 0.0f;
	float turnSpeed = DirectX::XMConvertToRadians(180);
};