#pragma once 

#include <DirectXMath.h> 
#include <Effekseer.h> 

// �G�t�F�N�g 
class Effect
{
public:
	//! �t�@�C���l�[���͉p��ŁI�I
	Effect(const char* filename);
	~Effect() {};

	// �Đ� 
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 scale = { 1, 1, 1 }, const DirectX::XMFLOAT3 angle = { 0, 0, 0 });

	// ��~ 
	void Stop(Effekseer::Handle handle);

	// ���W�ݒ� 
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	// �X�P�[���ݒ� 
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	// �p�x�ݒ�
	void SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation);

private:
	Effekseer::EffectRef effekseerEffect;
};