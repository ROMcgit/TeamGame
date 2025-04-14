#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// ��������@�����U���g
class G4_OssanTataki_Result : public Scene
{
public:
	G4_OssanTataki_Result() {};
	~G4_OssanTataki_Result() override {};

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