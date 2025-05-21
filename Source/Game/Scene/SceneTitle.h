#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// �^�C�g���V�[��
class SceneTitle : public Scene
{
public:
	SceneTitle() {};
	~SceneTitle() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	std::unique_ptr<Sprite> title;
	std::unique_ptr<Sprite> titleText;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // �t�F�[�h��ݒ肵����

	std::unique_ptr<Sprite> text;
	float textOpacity = 1.0f;
	bool textOpacityUp = false;
	float textTimer = 0.0f;
};