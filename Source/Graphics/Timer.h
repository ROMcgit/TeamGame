#pragma once
#include "Sprite.h"
#include <memory>

class Graphics;
class Text;

class Timer
{
public:
	Timer(bool countdown = false, int timeM = 0, int timeS = 0);
	~Timer();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Graphics& graphics, DirectX::XMFLOAT2 position);

private:
	float timeM = 0; // ��
	float timeS = 0; // �b

	std::unique_ptr<Text> timeMSprite;
	std::unique_ptr<Text> timeSSprite;

	bool countdown = false; // �J�E���g�_�E�����邩
};