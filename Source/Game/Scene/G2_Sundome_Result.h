#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class Fade;

// 寸止めリザルト
class G2_Sundome_Result : public Scene
{
public:
	G2_Sundome_Result() {};
	~G2_Sundome_Result() override {};

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