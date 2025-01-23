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

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Graphics& graphics, DirectX::XMFLOAT2 position = { 30, 0 }, DirectX::XMFLOAT4 color = { 1, 1, 1, 1});

	// 分を取得
	float GetTimeM() { return (int)timeM; }

	// 秒を取得
	float GetTimeS() { return (int)timeS; }

private:
	float timeM = 0; // 分
	float timeS = 0; // 秒

	std::unique_ptr<Text> timeMSprite;
	std::unique_ptr<Text> timeSSprite;
	std::unique_ptr<Sprite> timeColon; // タイムのセミコロン

	bool countdown = false; // カウントダウンするか
};