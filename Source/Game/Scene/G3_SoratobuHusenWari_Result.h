#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Graphics/Text.h"

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
	// �X�R�A�摜�̍X�V����
	void UpdateScoreSprite(float elapsedTime);

	int   scoreStep = 0;
	float scoreTimer = 0.0f;
	bool scoreDirector = false;

private:
	std::unique_ptr<Sprite> backGround;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // �t�F�[�h��ݒ肵����

	std::unique_ptr<Text> score;
	DirectX::XMFLOAT2 scorePos = { 0, 0 };
	float startScorePosX;
};