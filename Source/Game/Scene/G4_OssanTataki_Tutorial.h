#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// おっさん叩きリザルト
class G4_OssanTataki_Tutorial : public Scene
{
public:
	G4_OssanTataki_Tutorial() {};
	~G4_OssanTataki_Tutorial() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Fade> fade;
	bool setFade = false; // フェードを設定したか
};