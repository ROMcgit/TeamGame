#pragma once

#include <vector>
#include "Yazirusi.h"
#include <set>

// 弾丸マネージャー
class YazirusiManager
{
public:
	YazirusiManager();
	~YazirusiManager();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 弾丸登録
	void Register(Yazirusi* projectile);

	// 弾丸全削除
	void Clear();

	// 弾丸数取得
	int GetYazirusiCount() const { return static_cast<int>(projectiles.size()); }

	// 弾丸取得
	Yazirusi* GetYazirusi(int index) { return projectiles.at(index); }

	// 弾丸削除
	void Remove(Yazirusi* projectile);

private:
	std::vector<Yazirusi*> projectiles;
	std::set<Yazirusi*> removes;
};