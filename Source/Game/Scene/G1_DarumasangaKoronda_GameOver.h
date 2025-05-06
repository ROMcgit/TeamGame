#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// ����܂��񂪓]�񂾃Q�[���I�[�o�[
class G1_DarumasangaKoronda_GameOver : public Scene
{
public:
	G1_DarumasangaKoronda_GameOver() {};
	~G1_DarumasangaKoronda_GameOver() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> gameOver;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // �t�F�[�h��ݒ肵����
};