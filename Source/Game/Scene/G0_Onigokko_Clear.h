#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// �S�������N���A
class G0_Onigokko_Clear : public Scene
{
public:
	G0_Onigokko_Clear() {};
	~G0_Onigokko_Clear() override {};

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