#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// ���ԕ��D���胊�U���g
class G3_SoratobuHusenWari_Result : public Scene
{
public:
	G3_SoratobuHusenWari_Result() {};
	~G3_SoratobuHusenWari_Result() override {};

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