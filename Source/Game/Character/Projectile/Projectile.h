#pragma once

#include "Graphics/Shader.h"
#include "Game/Effect/Effect.h"
#include "../GameObjectBase.h"
#include <stdexcept>
#include <algorithm>

// 前方宣言
class ProjectileManager;

// 弾丸
class Projectile :public GameObjectBase
{
public:
	Projectile(ProjectileManager* manager);
	virtual ~Projectile() {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	// エフェクトのハンドルを取得
	virtual Effekseer::Handle GetHandle() { return -1; }

	// 破棄
	void Destroy();

	/*****************************************************************************************************************************/
		/*! ゲッター */
#if 1

	// 方向取得
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }

	//-----------------------------------------------------------------------------//

		// 不透明度を変えているかを取得
	bool GetOpacityChange() { return opacityChange; }

	// エミッシブの色を変えているかを取得
	bool GetEmissiveColorChange() { return emissiveColorChange; }

	// エミッシブの強さを変えているかを取得
	bool GetEmissiveStrengthChange() { return emissiveStrengthChange; }

#endif

protected:
	ProjectileManager* manager = nullptr;

	float lifeTimer = 1.0f; // 生存時間
};