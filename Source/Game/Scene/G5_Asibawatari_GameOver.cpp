#include "Graphics/Graphics.h"
#include "Game/Scene/G5_Asibawatari_GameOver.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Fade.h"

// ������
void G5_Asibawatari_GameOver::Initialize()
{
	// �X�v���C�g������
	sprite = std::make_unique<Sprite>("Data/Sprite/Title.png");

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);
}

// �I����
void G5_Asibawatari_GameOver::Finalize()
{
}

// �X�V����
void G5_Asibawatari_GameOver::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	fade->Update(elapsedTime);

	// �Ȃɂ��{�^�����������烍�[�f�B���O�V�[��������ŃQ�[���V�[���֐؂�ւ�
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B;
	if (gamePad.GetButtonDown() & anyButton && !setFade && !fade->GetFade())
	{
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			0.5f, 0.2f);

		setFade = true;
	}
	else if (setFade && !fade->GetFade())
	{
		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

		// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}
}

// �`�揈��
void G5_Asibawatari_GameOver::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A&�����_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; //RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2D�X�v���C�g�`��
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		// �^�C�g���X�v���C�g�`��
		sprite->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		fade->Render(dc, graphics);
	}
}