#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// ���ԕ��D����`���[�g���A��
class G3_SoratobuHusenWari_Tutorial : public Scene
{
public:
	G3_SoratobuHusenWari_Tutorial() {};
	~G3_SoratobuHusenWari_Tutorial() override {};

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