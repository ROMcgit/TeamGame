#include "Graphics/Graphics.h"
#include "EffectManager.h"

// ������
void EffectManager::Initialize()
{
	Graphics& graphics = Graphics::Instance();

	// Effekseer�����_������
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(),
		graphics.GetDeviceContext(), 2048);

	// Effekseer�}�l�[�W���[����
	effekseerManager = Effekseer::Manager::Create(2048);

	// Effekseer�����_���̊e��ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK)
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
	// Effekseer���ł̃��[�_�[�̐ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK�j 
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

	// Effekseer��������W�n�Ōv�Z���� 
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// �I����
void EffectManager::Finalize()
{
	// EffekseerManager�Ȃǂ̓X�}�[�g�|�C���^�ɂ���Ĕj�������̂ŉ������Ȃ�
}

// �X�V����
void EffectManager::Update(float elapsedTime)
{
	// �G�t�F�N�g�X�V����(�����ɂ̓t���[���̌o�ߎ��Ԃ�n��)
	effekseerManager->Update(elapsedTime * 60.0f);
}

// �`�揈��
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// �r���[&�v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	// Effekseer�`��J�n
	effekseerRenderer->BeginRendering();

	// Effeckseer�`����s
	// �}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬���A
	// Draw()�֐������s���鏇���Ő���ł�����
	effekseerManager->Draw();

	// Effekseer�`��I��
	effekseerRenderer->EndRendering();
}

// �G�t�F�N�g�v���C
Effekseer::Handle EffectManager::PlayEffect(const char* filename, const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale)
{
	auto effect = std::make_shared<Effect>(filename);
	Effekseer::Handle handle = effect->Play(position, scale);
	activeEffects[handle] = effect;
	return handle;
}

// �G�t�F�N�g�X�g�b�v
void EffectManager::StopEffect(Effekseer::Handle handle)
{
	if (activeEffects.find(handle) != activeEffects.end())
	{
		activeEffects[handle]->Stop(handle);
		activeEffects.erase(handle);
	}
}

// �G�t�F�N�g�ʒu�X�V����
void EffectManager::UpdateEffectPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	if (activeEffects.find(handle) != activeEffects.end())
	{
		activeEffects[handle]->SetPosition(handle, position);
	}
}

// �G�t�F�N�g�X�P�[���X�V����
void EffectManager::UpdateEffectScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	if (activeEffects.find(handle) != activeEffects.end())
	{
		activeEffects[handle]->SetScale(handle, scale);
	}
}

// �G�t�F�N�g�p�x�X�V����
void EffectManager::UpdateEffectRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation)
{
	if (activeEffects.find(handle) != activeEffects.end())
	{
		activeEffects[handle]->SetRotation(handle, rotation);
	}
}

// �G�t�F�N�g�폜
void EffectManager::Cleanup()
{
	for (auto& effect : activeEffects)
	{
		// �S�ẴG�t�F�N�g���~
		effekseerManager->StopEffect(effect.first);
	}
	// �}�b�v���N���A
	activeEffects.clear();
}
