#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <unordered_map>
#include <memory>
#include "Effect.h"

// エフェクトマネージャー
class EffectManager
{
private:
	EffectManager() {};
	~EffectManager() {};
public:
	// 唯一のインスタンス取得
	static  EffectManager& Instance()
	{
		static  EffectManager instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// Effeckseerマネージャーの取得
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

	// エフェクトプレイ
	Effekseer::Handle PlayEffect(const char* filename, const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale = { 1, 1, 1 });

	// エフェクトストップ
	void StopEffect(Effekseer::Handle handle);

	// エフェクト位置更新処理
	void UpdateEffectPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	// エフェクトスケール更新処理
	void UpdateEffectScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	// エフェクト角度更新処理
	void UpdateEffectRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation);

	// エフェクト削除
	void Cleanup();

private:
	Effekseer::ManagerRef          effekseerManager;
	EffekseerRenderer::RendererRef effekseerRenderer;
	std::unordered_map<Effekseer::Handle, std::shared_ptr<Effect>> activeEffects;
};