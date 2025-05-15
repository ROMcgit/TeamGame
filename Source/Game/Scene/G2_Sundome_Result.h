#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Text.h"

class Fade;

// ���~�߃��U���g
class G2_Sundome_Result : public Scene
{
public:
	G2_Sundome_Result() {};
	~G2_Sundome_Result() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	// �X�R�A�̉��o����
	void DirectorScore(float elapsedTime);

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Sprite> result;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // �t�F�[�h��ݒ肵����

	std::unique_ptr<Text> text[4];
	DirectX::XMFLOAT2 textPos[4];
	float startTextPosX;
	int textNum = 0;
	float totalScore = 0;

	int   directorStep = 0;
	float directorTime = 0.0f;
	bool  directorFinish = false;
};