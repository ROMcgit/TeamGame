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
	void Render(ID3D11DeviceContext* dc, Graphics& graphics, DirectX::XMFLOAT2 position = { 30, 0 }, DirectX::XMFLOAT4 color = { 1, 1, 1, 1});

	// ����int�^�Ŏ擾
	static float GetTimeM_Int() { return (int)timeM; }

	// �b��int�^�Ŏ擾
	static float GetTimeS_Int() { return (int)timeS; }

private:
	static float timeM; // ��
	static float timeS; // �b

	std::unique_ptr<Text> timeMSprite;
	std::unique_ptr<Text> timeSSprite;
	std::unique_ptr<Sprite> timeColon; // �^�C���̃Z�~�R����

	bool countdown = false; // �J�E���g�_�E�����邩
};