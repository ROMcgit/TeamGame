#include "Graphics/Graphics.h"
#include "Game/Scene/SceneTitle.h"
#include "SceneGameSelect.h"
#include "Game/Scene/SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Fade.h"
#include "Audio/BgmManager.h"
#include "Audio/SoundManager.h"

// ������
void SceneTitle::Initialize()
{
	BgmManager& bgm = BgmManager::Instance();
	bgm.LoadBgm("�^�C�g��", "Data/Audio/Bgm/0.Title.wav");
	bgm.PlayBgm("�^�C�g��", 0.7f);

	SoundManager::Instance().LoadSound("����", "Data/Audio/Sound/Decision.wav");

	// �X�v���C�g������
	sprite = std::make_unique<Sprite>("Data/Sprite/Title.png");

	text = std::make_unique<Sprite>("Data/Sprite/TitleButton.png");

	fade = std::make_unique<Fade>();
	fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
		1.0f, 0.0f,
		0.5f, 0.2f);
}

// �I����
void SceneTitle::Finalize()
{
}

// �X�V����
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	fade->Update(elapsedTime);

	// �Ȃɂ��{�^�����������烍�[�f�B���O�V�[��������ŃQ�[���V�[���֐؂�ւ�
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y |
		GamePad::BTN_START |
		GamePad::BTN_SPACE;
	if (gamePad.GetButtonDown() & anyButton && !setFade && !fade->GetFade())
	{
		fade->SetFade(DirectX::XMFLOAT3(0, 0, 0),
			0.0f, 1.0f,
			0.5f, 0.5f);

		SoundManager::Instance().PlaySound("����");

		setFade = true;
	}
	else if (setFade && !fade->GetFade())
	{
		BgmManager::Instance().UnloadBgm("�^�C�g��");

		std::unique_ptr<SceneLoading> loadingScene = std::make_unique<SceneLoading>(std::make_unique<SceneGameSelect>());

		// �V�[���}�l�[�W���[�Ƀ��[�f�B���O�V�[���ւ̐؂�ւ����w��
		SceneManager::Instance().ChangeScene(std::move(loadingScene));
	}

	if(!fade->GetFade())
		textOpacity += (1.5f * elapsedTime) * (textOpacityUp ? 1 : -1);

	if (textOpacity <= 0.0f || textOpacity >= 1.0f)
	{
		textOpacity = std::clamp(textOpacity, 0.0f, 1.0f);

		textOpacityUp = !textOpacityUp;
	}

	if (setFade)
		textTimer += elapsedTime;
}

// �`�揈��
void SceneTitle::Render()
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

		//! �\������
		const float viewTimer = 0.1f;

		textureWidth = static_cast<float>(text->GetTextureWidth());
		textureHeight = static_cast<float>(text->GetTextureHeight());

		//! �^�C�g������
		if (!setFade)
		{
			text->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, textOpacity);
		}
		else if (fmod(textTimer, viewTimer * 2) < viewTimer)
		{
			text->Render(dc,
				0, 0,
				screenWidth, screenHeight,
				0, 0,
				textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}

		fade->Render(dc, graphics);
	}
}