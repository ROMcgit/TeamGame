#include "Timer.h"
#include "Text.h"

//! ��
float Timer::timeM = 0;
//! �b
float Timer::timeS = 0;

Timer::Timer(bool countdown, int timeM, int timeS)
{
	this->countdown = countdown;
	this->timeM = timeM;
	this->timeS = timeS;

	timeMSprite = std::make_unique<Text>();
	timeSSprite = std::make_unique<Text>();

	//! �^�C���̃Z�~�R����
	timeColon = std::make_unique<Sprite>("Data/Font/Colon.png");
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

			//! �J�E���g�_�E���Ȃ�60�A����Ȃ��Ȃ�0�ɂ���
			countdown ? timeS = 60 : timeS = 0;
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
void Timer::Render(ID3D11DeviceContext* dc, Graphics& graphics, DirectX::XMFLOAT2 position, DirectX::XMFLOAT4 color)
{
	//! ���̕`��
	timeMSprite->RenderThousand(dc, false, (int)timeM, false, false, true, position.x, position.y, 8.0f, 8.0f, 0, 28.0f, color.x, color.y, color.z, color.w);
	//! �b�̕`��
	timeSSprite->RenderThousand(dc, false, (int)timeS, false, false, true, position.x + 80, position.y, 8.0f, 8.0f, 0, 28.0f, color.x, color.y, color.z, color.w);

	//! �e�N�X�`���̑傫��
	float textureWidth  = static_cast<float>(timeColon->GetTextureWidth());
	float textureHeight = static_cast<float>(timeColon->GetTextureHeight());

	//! �^�C���̃Z�~�R�����̕`��
	timeColon->Render(dc,
		position.x + 24, position.y,
		40, 40,
		0, 0,
		textureWidth, textureHeight,
		0,
		color.x, color.y, color.z, color.w);
}