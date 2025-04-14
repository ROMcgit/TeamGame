#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// ����n��Q�[���I�[�o�[
class G5_Asibawatari_GameOver : public Scene
{
public:
	G5_Asibawatari_GameOver() {};
	~G5_Asibawatari_GameOver() override {};

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