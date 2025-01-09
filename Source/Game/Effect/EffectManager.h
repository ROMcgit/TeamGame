#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <unordered_map>
#include <memory>
#include "Effect.h"

// �G�t�F�N�g�}�l�[�W���[
class EffectManager
{
private:
	EffectManager() {};
	~EffectManager() {};
public:
	// �B��̃C���X�^���X�擾
	static  EffectManager& Instance()
	{
		static  EffectManager instance;
		return instance;
	}

	// ������
	void Initialize();

	// �I����
	void Finalize();

	// �X�V��������
	void Update(float elapsedTime);

	// �`�揈��
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// Effeckseer�}�l�[�W���[�̎擾
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

	// �G�t�F�N�g�v���C
	Effekseer::Handle PlayEffect(const char* filename, const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale = { 1, 1, 1 });

	// �G�t�F�N�g�X�g�b�v
	void StopEffect(Effekseer::Handle handle);

	// �G�t�F�N�g�ʒu�X�V����
	void UpdateEffectPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	// �G�t�F�N�g�X�P�[���X�V����
	void UpdateEffectScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	// �G�t�F�N�g�p�x�X�V����
	void UpdateEffectRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation);

	// �G�t�F�N�g�폜
	void Cleanup();

private:
	Effekseer::ManagerRef          effekseerManager;
	EffekseerRenderer::RendererRef effekseerRenderer;
	std::unordered_map<Effekseer::Handle, std::shared_ptr<Effect>> activeEffects;
};