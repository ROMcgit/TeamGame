#include "Timer.h"
#include "Text.h"

//! 分
float Timer::timeM = 0;
//! 秒
float Timer::timeS = 0;

Timer::Timer(bool countdown, int timeM, int timeS)
{
	this->countdown = countdown;
	this->timeM = timeM;
	this->timeS = timeS;

	timeMSprite = std::make_unique<Text>();
	timeSSprite = std::make_unique<Text>();

	//! タイムのセミコロン
	timeColon = std::make_unique<Sprite>("Data/Font/Colon.png");
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

			//! カウントダウンなら60、じゃないなら0にする
			countdown ? timeS = 60 : timeS = 0;
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
void Timer::Render(ID3D11DeviceContext* dc, Graphics& graphics, DirectX::XMFLOAT2 position, DirectX::XMFLOAT4 color)
{
	//! 分の描画
	timeMSprite->RenderThousand(dc, false, (int)timeM, false, false, true, position.x, position.y, 8.0f, 8.0f, 0, 28.0f, color.x, color.y, color.z, color.w);
	//! 秒の描画
	timeSSprite->RenderThousand(dc, false, (int)timeS, false, false, true, position.x + 80, position.y, 8.0f, 8.0f, 0, 28.0f, color.x, color.y, color.z, color.w);

	//! テクスチャの大きさ
	float textureWidth  = static_cast<float>(timeColon->GetTextureWidth());
	float textureHeight = static_cast<float>(timeColon->GetTextureHeight());

	//! タイムのセミコロンの描画
	timeColon->Render(dc,
		position.x + 24, position.y,
		40, 40,
		0, 0,
		textureWidth, textureHeight,
		0,
		color.x, color.y, color.z, color.w);
}