#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// �S�������`���[�g���A��
class G0_Onigokko_Tutorial : public Scene
{
public:
	G0_Onigokko_Tutorial() {};
	~G0_Onigokko_Tutorial() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // �t�F�[�h��ݒ肵����
};