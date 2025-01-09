#pragma once 

#include <DirectXMath.h> 
#include <Effekseer.h> 

// エフェクト 
class Effect
{
public:
	//! ファイルネームは英語で！！
	Effect(const char* filename);
	~Effect() {};

	// 再生 
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 scale = { 1, 1, 1 }, const DirectX::XMFLOAT3 angle = { 0, 0, 0 });

	// 停止 
	void Stop(Effekseer::Handle handle);

	// 座標設定 
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	// スケール設定 
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	// 角度設定
	void SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation);

private:
	Effekseer::EffectRef effekseerEffect;
};