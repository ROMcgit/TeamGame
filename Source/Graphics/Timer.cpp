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

// 更新処理
void Timer::Update(float elapsedTime)
{
	//! カウントダウンするかによって、処理を変える
	countdown ? timeS -= elapsedTime : timeS += elapsedTime;

	//! カウントダウンではなく60より大きい、またはカウントダウンで、0より小さいなら
	if ((!countdown && (int)timeS >= 60) || (countdown && (int)timeS < 0))
	{
		//! 分が0より大きいかつ99より小さいなら
		if (((int)timeM > 0 && (int)timeM < 99))
		{
			//! カウントダウンなら分を減らす、じゃないなら増やす
			countdown ? timeM-- : timeM++;

			//! カウントダウンなら59、じゃないなら0にする
			countdown ? timeS = 59 : timeS = 0;
		}
		//! カウントを止める
		else
		{
			//! カウントダウンなら分を0、じゃないなら99
			countdown ? timeM = 0 : timeM = 99;

			//! カウントダウンなら0、じゃないなら59にする
			countdown ? timeS = 0 : timeS = 59;
		}
	}
}

// 描画処理
void Timer::Render(ID3D11DeviceContext* dc, Graphics& graphics, DirectX::XMFLOAT2 position = { 30, 0 })
{
	//! 分の描画
	timeMSprite->RenderThousand(dc, false, (int)timeM, false, false, true, position.x, position.y);
	//! 秒の描画
	timeSSprite->RenderThousand(dc, false, (int)timeS, false, false, true, position.x + 80, position.y);
}