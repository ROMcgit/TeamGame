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

	// �����擾
	float GetTimeM() { return (int)timeM; }

	// �b���擾
	float GetTimeS() { return (int)timeS; }

private:
	float timeM = 0; // ��
	float timeS = 0; // �b

	std::unique_ptr<Text> timeMSprite;
	std::unique_ptr<Text> timeSSprite;
	std::unique_ptr<Sprite> timeColon; // �^�C���̃Z�~�R����

	bool countdown = false; // �J�E���g�_�E�����邩
};