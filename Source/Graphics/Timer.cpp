#include "Timer.h"
#include "Text.h"

Timer::Timer(bool countdown, int timeM, int timeS)
{
	this->countdown = countdown;
	this->timeM = timeM;
	this->timeS = timeS;

	timeMSprite = std::make_unique<Text>();
	timeSSprite = std::make_unique<Text>();
}

Timer::~Timer()
{
}

// �X�V����
void Timer::Update(float elapsedTime)
{
	//! �J�E���g�_�E�����邩�ɂ���āA������ς���
	countdown ? timeS -= elapsedTime : timeS += elapsedTime;

	//! �J�E���g�_�E���ł͂Ȃ�60���傫���A�܂��̓J�E���g�_�E���ŁA0��菬�����Ȃ�
	if ((!countdown && (int)timeS >= 60) || (countdown && (int)timeS < 0))
	{
		//! ����0���傫������99��菬�����Ȃ�
		if (((int)timeM > 0 && (int)timeM < 99))
		{
			//! �J�E���g�_�E���Ȃ番�����炷�A����Ȃ��Ȃ瑝�₷
			countdown ? timeM-- : timeM++;

			//! �J�E���g�_�E���Ȃ�59�A����Ȃ��Ȃ�0�ɂ���
			countdown ? timeS = 59 : timeS = 0;
		}
		//! �J�E���g���~�߂�
		else
		{
			//! �J�E���g�_�E���Ȃ番��0�A����Ȃ��Ȃ�99
			countdown ? timeM = 0 : timeM = 99;

			//! �J�E���g�_�E���Ȃ�0�A����Ȃ��Ȃ�59�ɂ���
			countdown ? timeS = 0 : timeS = 59;
		}
	}
}

// �`�揈��
void Timer::Render(ID3D11DeviceContext* dc, Graphics& graphics, DirectX::XMFLOAT2 position = { 30, 0 })
{
	//! ���̕`��
	timeMSprite->RenderThousand(dc, false, (int)timeM, false, false, true, position.x, position.y);
	//! �b�̕`��
	timeSSprite->RenderThousand(dc, false, (int)timeS, false, false, true, position.x + 80, position.y);
}