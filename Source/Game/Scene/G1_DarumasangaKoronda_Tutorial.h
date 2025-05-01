#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// ����܂��񂪓]�񂾃`���[�g���A��
class G1_DarumasangaKoronda_Tutorial : public Scene
{
public:
	G1_DarumasangaKoronda_Tutorial() {};
	~G1_DarumasangaKoronda_Tutorial() override {};

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