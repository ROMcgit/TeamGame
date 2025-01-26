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

	// 分をint型で取得
	static float GetTimeM_Int() { return (int)timeM; }

	// 秒をint型で取得
	static float GetTimeS_Int() { return (int)timeS; }

private:
	static float timeM; // 分
	static float timeS; // 秒

	std::unique_ptr<Text> timeMSprite;
	std::unique_ptr<Text> timeSSprite;
	std::unique_ptr<Sprite> timeColon; // タイムのセミコロン

	bool countdown = false; // カウントダウンするか
};